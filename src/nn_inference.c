//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-28
// Description:     Neural network inference - Matrix multiplication
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "const_values.h"
#include "nn_inference.h"
#include "uart.h"

uint64_t* sensor_data_base_addr = (uint64_t*)SENSOR_ENCODINGS_BASE_ADDR;    // Base address of sensor data
uint64_t* pred_data_base_addr = (uint64_t*)SENSOR_PRED_BASE_ADDR;           // Base address of prediction data

static const uint16_t temp_scale_factor = 0xFF; // Scale factor for temperature encoding
static const uint16_t temp_shift_width = 8;     // Shift width for temperature encoding
static const int8_t temp_zero_point = -64;      // Zero point for temperature encoding

static const uint16_t power_scale_factor = 0xF; // Scale factor for power encoding
static const uint16_t power_shift_width = 8;    // Shift width for power encoding
static const int8_t power_zero_point = -32;     // Zero point for power encoding

static const uint16_t output_scale_factor = 0xFF; // Scale factor for output dequantization
static const uint16_t output_shift_width = 8;     // Shift width for output dequantization
static const int32_t output_zero_point = 0;       // Zero point for output dequantization

// Fixed-point quantization: Convert uint32_t to int8_t
int8_t quantize_u32_to_i8(uint32_t value, uint16_t scale_factor, uint16_t shift_width, int8_t zero_point)
{
    // Apply fixed-point scaling (Q8.24 format)
    int64_t scaled_value = ((int64_t)value * scale_factor) >> shift_width;

    // Add zero point
    int32_t result = (int32_t)scaled_value + zero_point;

    // Clamp to int8_t range [-128, 127]
    if (result > 127)
        return 127;
    else if (result < -128)
        return -128;

    return (int8_t)result;
}

// Dequantize int32_t to uint32_t (for output)
uint32_t dequantize_i32_to_u32(int32_t value, uint16_t scale_factor, uint16_t shift_width, int32_t zero_point)
{
    // Remove zero point
    int64_t centered = (int64_t)value - zero_point;

    // Apply inverse scaling (scale up, then divide)
    uint32_t result = (uint32_t)((centered << shift_width) / scale_factor);

    // Ensure no negative values for uint32_t
    return (result > 0) ? result : 0;
}

// ReLU activation function for int32_t
void relu_i32(int32_t* data, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (data[i] < 0)
        {
            data[i] = 0;
        }
    }
}

// Run inference with matrices from fixed memory locations
void run_model_inference(void)
{
    int8_t input_buffer[NUM_SENSORS * 2];
    int32_t hidden_buffer[HIDDEN_DIM];
    int32_t output_buffer[OUTPUT_DIM];

    // Wait for start signal
    while (*(volatile uint64_t*)CPU_START_FLAG_ADDR != CPU_COMPUTE_START_FLAG) {};

    // Read input from memory and quantize
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        uint64_t node_data = *(sensor_data_base_addr + i);
        uint32_t temp_encoding = (uint32_t)(node_data & 0x3FF);              // 10-bit encoding
        uint32_t power_encoding = (uint32_t)((node_data >> 36) & 0x3FFFFFF); // 26-bit encoding
        input_buffer[i] = quantize_u32_to_i8(temp_encoding, temp_scale_factor, temp_shift_width, temp_zero_point);
        input_buffer[i + NUM_SENSORS] = quantize_u32_to_i8(power_encoding, power_scale_factor, power_shift_width, power_zero_point);
    }

    // Initialize hidden buffer
    for (int i = 0; i < HIDDEN_DIM; i++)
    {
        hidden_buffer[i] = 0;
    }

    // Layer 1: Input x Weight -> Hidden
    for (int i = 0; i < HIDDEN_DIM; i++)
    {
        for (int j = 0; j < INPUT_DIM; j++)
        {
            hidden_buffer[i] += (int32_t)input_buffer[j] * (int32_t)layer_0_weights[j * HIDDEN_DIM + i];
        }
    }

    // Apply ReLU activation
    relu_i32(hidden_buffer, HIDDEN_DIM);

    // Initalize output buffer
    for (int i = 0; i < OUTPUT_DIM; i++)
    {
        output_buffer[i] = 0;
    }

    // Layer 2: Hidden x Weight -> Output
    for (int i = 0; i < OUTPUT_DIM; i++)
    {
        for (int j = 0; j < HIDDEN_DIM; j++)
        {
            output_buffer[i] += (int32_t)hidden_buffer[j] * (int32_t)layer_1_weights[j * OUTPUT_DIM + i];
        }
    }

    for (int i = 0; i < OUTPUT_DIM; i++)
    {
        uint32_t pred_encoding = dequantize_i32_to_u32(output_buffer[i], output_scale_factor, output_shift_width, output_zero_point);
        *(pred_data_base_addr + i) = (uint64_t)pred_encoding; // Store prediction
    }

    // Set finish signal
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_COMPUTE_FINISH_FLAG;

    return;
}
