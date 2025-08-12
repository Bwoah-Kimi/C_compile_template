#include <stdint.h>
#include <stdbool.h>

#include "quantized_model_inference.h"
// Include the PTQ model instead of the old quantized model
#include "ptq_mlp_28_32_32_14_w8_a8_model.h"

// Clamp to target range with proper bounds checking
static inline int8_t clamp_to_target_range(int32_t value) {
    if (value > scaling_params.target_max) return scaling_params.target_max;
    if (value < scaling_params.target_min) return scaling_params.target_min;
    return (int8_t)value;
}

// Quantize input data using integer-only bit-shift scaling
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

// Dequantize output data to convert back to UINT10 thermal traces
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

// PTQ Linear layer forward pass
void ptq_linear_forward(const ptq_layer_config_t* layer, const int32_t* input, int32_t* output) {
    for (uint16_t out_idx = 0; out_idx < layer->output_size; out_idx++) {
        int64_t accumulator = 0;

        // Matrix multiplication: output = input @ weight.T + bias
        for (uint16_t in_idx = 0; in_idx < layer->input_size; in_idx++) {
            uint32_t weight_idx = out_idx * layer->input_size + in_idx;
            int8_t weight = layer->weights[weight_idx];
            int32_t input_val = input[in_idx];

            // Integer matrix multiplication
            accumulator += (int64_t)input_val * weight;
        }

        // Add bias
        accumulator += (int64_t)layer->bias[out_idx];

        // Store result as int32 (this matches the Python implementation)
        output[out_idx] = clamp_int32(accumulator);
    }
}

// PTQ Quantization layer forward pass
void ptq_quantization_forward(const ptq_quant_config_t* quant, const int32_t* input, int32_t* output) {
    for (uint16_t ch = 0; ch < quant->num_channels; ch++) {
        int16_t scale_int = quant->scale_int[ch];
        int8_t zero_point = quant->zero_point[ch];

        // Apply quantization formula: output = (input + zero_point) / scale_int
        int64_t input_with_zp = (int64_t)input[ch] + (int64_t)zero_point;

        // Integer division (matches Python // operator)
        int32_t quantized = (int32_t)(input_with_zp / scale_int);

        // Clamp to quantization range [-128, 127] for 8-bit
        if (quantized > 127) quantized = 127;
        if (quantized < -128) quantized = -128;

        output[ch] = quantized;
    }
}

// ReLU activation function for int32 values
void relu_activation_int32(int32_t* data, uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        if (data[i] < 0) {
            data[i] = 0;
        }
    }
}

// Main PTQ inference function - matches Python forward_integer
int ptq_mlp_inference(const int32_t* input_int32, int32_t* output_int32) {
    // Temporary buffers for intermediate activations
    static int32_t layer_input[MAX_NEURONS];  // Max hidden size
    static int32_t layer_output[MAX_NEURONS];
    static int32_t quant_output[MAX_NEURONS];

    // Copy input to working buffer
    for (uint16_t i = 0; i < MLP_INPUT_SIZE; i++) {
        layer_input[i] = input_int32[i];
    }

    // Process through all hidden layers
    for (uint16_t layer_idx = 0; layer_idx < MLP_NUM_HIDDEN_LAYERS; layer_idx++) {
        // Linear transformation
        ptq_linear_forward(&ptq_layer_configs[layer_idx], layer_input, layer_output);

        // Quantization (compensation absorbed in scale_int)
        ptq_quantization_forward(&ptq_quant_configs[layer_idx], layer_output, quant_output);

        // ReLU activation
        relu_activation_int32(quant_output, ptq_layer_configs[layer_idx].output_size);

        // Copy quantized output to input buffer for next layer
        for (uint16_t i = 0; i < ptq_layer_configs[layer_idx].output_size; i++) {
            layer_input[i] = quant_output[i];
        }
    }

    // Output layer (linear transformation)
    ptq_linear_forward(&ptq_layer_configs[MLP_NUM_HIDDEN_LAYERS], layer_input, layer_output);

    // Output quantization
    ptq_quantization_forward(&ptq_quant_configs[2], layer_output, output_int32);

    return 0; // Success
}

// Wrapper function to convert from int8 input to int32 and call PTQ inference
int quantized_mlp_inference(const QuantizedMLP* model, const int8_t* input_quantized, int32_t* output_fixed) {
    // Convert int8 input to int32 (matches Python: x_int = torch.round(x).to(torch.int32))
    static int32_t input_int32[MLP_INPUT_SIZE];

    for (uint16_t i = 0; i < MLP_INPUT_SIZE; i++) {
        input_int32[i] = (int32_t)input_quantized[i];
    }

    // Call PTQ inference
    return ptq_mlp_inference(input_int32, output_fixed);
}

// Initialize the QuantizedMLP model structure (kept for compatibility)
void initialize_quantized_model(QuantizedMLP* model) {
    // Set model configuration to match PTQ model
    model->num_layers = MLP_NUM_LAYERS;
    model->num_hidden_layers = MLP_NUM_HIDDEN_LAYERS;
    model->input_size = MLP_INPUT_SIZE;
    model->output_size = MLP_OUTPUT_SIZE;
    model->activation_bits = MLP_ACTIVATION_BITS;
    model->weight_bits = MLP_WEIGHT_BITS;

    return;
}
