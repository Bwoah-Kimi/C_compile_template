#ifndef INIT_STANDARDIZATION_UNIT_H
#define INIT_STANDARDIZATION_UNIT_H

#include <stdint.h>

void init_standardization_unit(void);

// Function to create the quantization register file data
uint64_t create_quant_regfile_data(
    uint16_t thermal_scale_mult,
    uint8_t thermal_scale_shift,
    uint8_t thermal_zero_point,
    uint16_t power_scale_mult,
    uint8_t power_scale_shift,
    uint8_t power_zero_point);

// Function to create the dequantization register file data
uint64_t create_dequant_regfile_data(
    uint16_t dequant_scale,
    uint8_t dequant_shift,
    uint8_t dequant_zero);

#endif // INIT_STANDARDIZATION_UNIT_H