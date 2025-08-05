#include <stdint.h>
#include <stdbool.h>

#include "quantized_model_inference.h"
// #include "quantized_mlp_170_256_85_w8_a8_p1_model.h"
#include "quantized_mlp_28_64_14_w8_a8_p1_model.h"

// Clamp to target range with proper bounds checking
static inline int8_t clamp_to_target_range(int32_t value) {
    if (value > scaling_params.target_max) return scaling_params.target_max;
    if (value < scaling_params.target_min) return scaling_params.target_min;
    return (int8_t)value;
}

// Updated quantize_input_data using integer-only bit-shift scaling
void quantize_input_data(const uint32_t* power_traces, const uint16_t* thermal_traces,
    int8_t* quantized_input, uint16_t num_sensors) {
    uint16_t input_idx = 0;

    // Process power traces (first in the input vector)
    for (uint16_t i = 0; i < num_sensors; i++) {
        uint32_t power_raw = power_traces[i];

        // Clamp to expected range
        if (power_raw < scaling_params.power_data_min) power_raw = scaling_params.power_data_min;
        if (power_raw > scaling_params.power_data_max) power_raw = scaling_params.power_data_max;

        // Step 1: Subtract minimum (shift to zero-based)
        uint32_t power_zero_based = power_raw - scaling_params.power_data_min;

        // Step 2: Apply bit-shift scaling
        int32_t power_scaled;
        if (scaling_params.power_upscale) {
            power_scaled = (int32_t)(power_zero_based << scaling_params.power_scale_shift);
        }
        else {
            power_scaled = (int32_t)(power_zero_based >> scaling_params.power_scale_shift);
        }

        // Step 3: Add offset to shift to target range
        int32_t power_quantized = power_scaled + scaling_params.power_offset;

        // Step 4: Clamp to target range
        quantized_input[input_idx++] = clamp_to_target_range(power_quantized);
    }

    // Process thermal traces (after power traces in the input vector)
    for (uint16_t i = 0; i < num_sensors; i++) {
        uint16_t thermal_raw = thermal_traces[i];

        // Clamp to expected range (thermal is UINT10: 0-1023)
        if (thermal_raw < scaling_params.thermal_data_min) thermal_raw = scaling_params.thermal_data_min;
        if (thermal_raw > scaling_params.thermal_data_max) thermal_raw = scaling_params.thermal_data_max;

        // Step 1: Subtract minimum (shift to zero-based)
        uint16_t thermal_zero_based = thermal_raw - scaling_params.thermal_data_min;

        // Step 2: Apply bit-shift scaling
        int32_t thermal_scaled;
        if (scaling_params.thermal_upscale) {
            thermal_scaled = (int32_t)(thermal_zero_based << scaling_params.thermal_scale_shift);
        }
        else {
            thermal_scaled = (int32_t)(thermal_zero_based >> scaling_params.thermal_scale_shift);
        }

        // Step 3: Add offset to shift to target range
        int32_t thermal_quantized = thermal_scaled + scaling_params.thermal_offset;

        // Step 4: Clamp to target range
        quantized_input[input_idx++] = clamp_to_target_range(thermal_quantized);
    }
}

// Updated dequantize_output_data to convert back to UINT10 thermal traces
void dequantize_output_data(const int32_t* quantized_output, uint16_t* thermal_output, uint16_t output_size) {
    for (uint16_t i = 0; i < output_size; i++) {
        // Reverse the quantization process:
        // 1. Subtract offset to get scaled value
        int32_t thermal_scaled = quantized_output[i] - scaling_params.thermal_offset;

        // 2. Reverse bit-shift scaling
        uint16_t thermal_zero_based;
        if (scaling_params.thermal_upscale) {
            // Was left-shifted during quantization, so right-shift to reverse
            thermal_zero_based = (uint16_t)(thermal_scaled >> scaling_params.thermal_scale_shift);
        }
        else {
            // Was right-shifted during quantization, so left-shift to reverse
            thermal_zero_based = (uint16_t)(thermal_scaled << scaling_params.thermal_scale_shift);
        }

        // 3. Add back the minimum to get original range
        uint16_t thermal_raw = thermal_zero_based + scaling_params.thermal_data_min;

        // 4. Clamp to UINT10 range [0, 1023]
        if (thermal_raw > 1023) thermal_raw = 1023;

        thermal_output[i] = thermal_raw;
    }
}

// Fixed-point arithmetic helpers
int32_t fixed_point_multiply(int32_t a, int16_t b, uint8_t precision_bits) {
    // Multiply and then shift right to maintain precision
    int64_t result = (int64_t)a * b;
    return (int32_t)(result >> precision_bits);
}

static inline int8_t clamp_int8(int32_t value) {
    if (value > 127) return 127;
    if (value < -128) return -128;
    return (int8_t)value;
}

static inline int32_t clamp_int32(int64_t value) {
    if (value > 2147483647LL) return 2147483647;
    if (value < -2147483648LL) return -2147483648;
    return (int32_t)value;
}

// Pure integer quantized linear layer forward pass
void quantized_linear_forward(const QuantizedLinearLayer* layer, const int8_t* input, int32_t* output) {
    for (uint16_t out_idx = 0; out_idx < layer->out_features; out_idx++) {
        int64_t accumulator = 0;

        // Get scaling parameters for this output channel
        int16_t scale_int = layer->scale_int[out_idx];
        int8_t zero_point = layer->zero_point[out_idx];

        // Matrix multiplication with quantized weights and inputs
        for (uint16_t in_idx = 0; in_idx < layer->in_features; in_idx++) {
            uint32_t weight_idx = out_idx * layer->in_features + in_idx;
            int8_t quantized_weight = layer->int_weights[weight_idx];
            int8_t quantized_input = input[in_idx];

            // Dequantize weight: (quantized_weight - zero_point)
            int16_t dequant_weight = (int16_t)(quantized_weight - zero_point);

            // Multiply quantized input by dequantized weight
            // Both input and dequant_weight are small integers, safe to multiply
            accumulator += (int64_t)quantized_input * dequant_weight;
        }

        // Apply weight scaling: result = accumulator * scale_int
        // We keep this in higher precision to avoid overflow
        int64_t scaled_result = accumulator * scale_int;

        // Add bias (already in fixed-point format with precision_bits scaling)
        scaled_result += (int64_t)layer->bias_fixed[out_idx];

        // Store result as 32-bit fixed-point (with precision_bits fractional bits)
        output[out_idx] = clamp_int32(scaled_result);
    }
}

// Pure integer per-channel quantization forward pass
void quantization_layer_forward(const QuantizationLayer* quant, const int32_t* input, int8_t* output) {
    for (uint16_t ch = 0; ch < quant->num_channels; ch++) {
        // Get scaling parameters for this channel
        int16_t scale_int = quant->scale_int[ch];
        int8_t zero_point = quant->zero_point[ch];

        // Quantize: input * scale_int / (2^precision_bits) + zero_point
        int32_t scaled_input = fixed_point_multiply(input[ch], scale_int, quant->precision_bits);
        int32_t quantized_raw = scaled_input + (int32_t)zero_point;

        // Clamp to quantization range
        int8_t quantized = clamp_int8(quantized_raw);
        if (quantized < quant->qmin) quantized = quant->qmin;
        if (quantized > quant->qmax) quantized = quant->qmax;

        output[ch] = quantized;
    }
}

// ReLU activation function for 8-bit integers
void relu_activation_int8(int8_t* data, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        if (data[i] < 0) {
            data[i] = 0;
        }
    }
}

// Main inference function using pure integer arithmetic
int quantized_mlp_inference(const QuantizedMLP* model, const int8_t* input_quantized, int32_t* output_fixed) {
    // Temporary buffers for intermediate activations
    static int8_t layer_input_int8[MAX_NEURONS];
    static int32_t layer_output_fixed[MAX_NEURONS];
    static int8_t quant_output_int8[MAX_NEURONS];

    // Copy quantized input to working buffer
    for (uint16_t i = 0; i < model->input_size; i++) {
        layer_input_int8[i] = input_quantized[i];
    }

    // Process through all hidden layers
    for (uint16_t layer_idx = 0; layer_idx < model->num_hidden_layers; layer_idx++) {
        // Linear transformation (produces fixed-point output)
        quantized_linear_forward(&model->layers[layer_idx], layer_input_int8, layer_output_fixed);

        // Quantization (converts fixed-point to 8-bit integers)
        quantization_layer_forward(&model->quant_layers[layer_idx], layer_output_fixed, quant_output_int8);

        // ReLU activation (operates on 8-bit integers)
        relu_activation_int8(quant_output_int8, model->layers[layer_idx].out_features);

        // Copy quantized output to input buffer for next layer
        for (uint16_t i = 0; i < model->layers[layer_idx].out_features; i++) {
            layer_input_int8[i] = quant_output_int8[i];
        }
    }

    // Output layer (no quantization or activation) - produces fixed-point output
    quantized_linear_forward(&model->layers[model->num_hidden_layers], layer_input_int8, output_fixed);

    return 0; // Success
}

// Initialize the QuantizedMLP model structure
void initialize_quantized_model(QuantizedMLP* model) {
    // Set model configuration
    model->num_layers = MLP_NUM_LAYERS;
    model->num_hidden_layers = MLP_NUM_HIDDEN_LAYERS;
    model->input_size = MLP_INPUT_SIZE;
    model->output_size = MLP_OUTPUT_SIZE;
    model->activation_bits = MLP_ACTIVATION_BITS;
    model->weight_bits = MLP_WEIGHT_BITS;
    model->precision_bits = MLP_PRECISION_BITS;

    // Initialize layers using the layer_configs array
    for (uint16_t i = 0; i < model->num_layers; i++) {
        model->layers[i].int_weights = (int8_t*)layer_configs[i].weights;
        model->layers[i].bias_fixed = (int32_t*)layer_configs[i].bias;
        model->layers[i].scale_int = (int16_t*)layer_configs[i].scale_int;
        model->layers[i].zero_point = (int8_t*)layer_configs[i].zero_point;
        model->layers[i].in_features = layer_configs[i].input_size;
        model->layers[i].out_features = layer_configs[i].output_size;
        model->layers[i].weight_bits = MLP_WEIGHT_BITS;
        model->layers[i].precision_bits = MLP_PRECISION_BITS;
    }

    // Initialize quantization layers (only for hidden layers, not output)
    for (uint16_t i = 0; i < model->num_hidden_layers; i++) {
        model->quant_layers[i].scale_int = (int16_t*)quant_configs[i].scale_int;
        model->quant_layers[i].zero_point = (int8_t*)quant_configs[i].zero_point;
        model->quant_layers[i].num_channels = quant_configs[i].num_channels;
        model->quant_layers[i].output_bits = MLP_ACTIVATION_BITS;
        model->quant_layers[i].precision_bits = MLP_PRECISION_BITS;
        model->quant_layers[i].qmin = -128;
        model->quant_layers[i].qmax = 127;
    }
    return;
}

