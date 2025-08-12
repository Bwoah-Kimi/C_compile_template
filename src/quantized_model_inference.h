#ifndef QUANTIZED_MODEL_INFERENCE_H
#define QUANTIZED_MODEL_INFERENCE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_LAYERS 6
#define MAX_QUANT_LAYERS (MAX_LAYERS - 1)
#define MAX_NEURONS 256

// Hardware parameters structure for integer-only scaling
typedef struct {
    // Input data ranges (actual data, not sensor limits)
    uint32_t power_data_min;
    uint32_t power_data_max;
    uint16_t thermal_data_min;
    uint16_t thermal_data_max;

    // Scaling parameters (bit-shift operations only)
    uint8_t power_scale_shift;
    uint8_t thermal_scale_shift;
    bool power_upscale;      // True = left shift (<<), False = right shift (>>)
    bool thermal_upscale;

    // Offset parameters
    int16_t power_offset;
    int16_t thermal_offset;

    // Target quantization parameters
    int8_t target_min;
    int8_t target_max;
} hardware_scaling_params_t;

// Data types for pure integer inference
typedef struct {
    int8_t* int_weights;       // Quantized weights (8-bit)
    int32_t* bias_fixed;       // Bias in fixed-point format (bias * 2^precision_bits)
    int16_t* scale_int;        // Integer scaling factors (scale * 2^precision_bits)
    int8_t* zero_point;        // Zero points for weight quantization
    uint16_t in_features;
    uint16_t out_features;
    uint8_t weight_bits;
    uint8_t precision_bits;
} QuantizedLinearLayer;

typedef struct {
    int16_t* scale_int;        // Integer scaling factors (scale * 2^precision_bits)
    int8_t* zero_point;        // Zero points for activation quantization
    uint16_t num_channels;
    uint8_t output_bits;
    uint8_t precision_bits;
    int8_t qmin;
    int8_t qmax;
} QuantizationLayer;

typedef struct {
    QuantizedLinearLayer layers[MAX_LAYERS];
    QuantizationLayer quant_layers[MAX_LAYERS - 1]; // No quantization after output layer
    uint16_t num_layers;
    uint16_t num_hidden_layers;
    uint16_t input_size;
    uint16_t output_size;
    uint8_t activation_bits;
    uint8_t weight_bits;
    uint8_t precision_bits;
} QuantizedMLP;


// Function prototypes
void quantize_input_data(const uint32_t* power_traces, const uint16_t* thermal_traces, int8_t* quantized_input, uint16_t num_sensors);
void dequantize_output_data(const int32_t* quantized_output, uint16_t* thermal_output, uint16_t output_size);
int quantized_mlp_inference(const QuantizedMLP* model, const int8_t* input_quantized, int32_t* output_fixed);
void initialize_quantized_model(QuantizedMLP* model);
// void quantized_linear_forward(const QuantizedLinearLayer* layer, const int8_t* input, int32_t* output);
// void quantization_layer_forward(const QuantizationLayer* quant, const int32_t* input, int8_t* output);
// void relu_activation_int8(int8_t* data, uint16_t size);


#endif // QUANTIZED_MODEL_INFERENCE_H