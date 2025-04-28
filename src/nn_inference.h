//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Created Date:    2025-04-28
// Description:     Neural network inference header
//////////////////////////////////////////////////////////////////////////////////

#ifndef NN_INFERENCE_H
#define NN_INFERENCE_H

#include <stdint.h>

// Neural network dimensions
#define NUM_SENSORS 14
#define INPUT_DIM 2 * NUM_SENSORS // 14 temp + 14 power encodings
#define HIDDEN_DIM 64
#define OUTPUT_DIM NUM_SENSORS // Future temperature predictions

// Quantization functions
/**
 * Convert uint32_t to quantized int8_t
 * @param value Raw uint32_t value
 * @param scale_factor Scaling factor in fixed point
 * @param shift_width Bit shift for fixed-point scaling
 * @param zero_point Zero point offset for quantization
 * @return Quantized int8_t value
 */
int8_t quantize_u32_to_i8(uint32_t value, uint16_t scale_factor, uint16_t shift_width, int8_t zero_point);

/**
 * Dequantize int32_t result to uint32_t output
 * @param value Quantized int32_t value
 * @param scale_factor Scaling factor in fixed point
 * @param shift_width Bit shift for fixed-point scaling
 * @param zero_point Zero point offset for dequantization
 * @return Dequantized uint32_t value
 */
uint32_t dequantize_i32_to_u32(int32_t value, uint16_t scale_factor, uint16_t shift_width, int32_t zero_point);

/**
 * ReLU activation function for int32_t array
 * @param data Pointer to int32_t data array
 * @param size Size of the array
 */
void relu_i32(int32_t* data, int size);

/**
 * Run neural network inference
 */
void run_model_inference(void);

#endif  // NN_INFERENCE_H