//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     C Code Template for RISC-V
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "init_standardization_unit.h"

void init_standardization_unit(void)
{
    // Base addresses for quantization and dequantization registers
    uint64_t *quant_regfile_base_addr = (uint64_t *)0x60002018;   // Example base address
    uint64_t *dequant_regfile_base_addr = (uint64_t *)0x60002118; // Example base address

    // Number of sensors to configure
    const uint8_t num_sensors = 16; // Example - replace with actual count

    // Configure each sensor's quantization parameters
    for (uint8_t i = 0; i < num_sensors; i++)
    {
        // Example values - replace with actual calibration values
        uint16_t thermal_scale_mult = 0x0100 + i; // Different value per sensor
        uint8_t thermal_scale_shift = i;
        uint8_t thermal_zero_point = 40 + i;

        uint16_t power_scale_mult = 0x0200 + i;
        uint8_t power_scale_shift = 10 + i;
        uint8_t power_zero_point = 20 + i;

        uint64_t quant_data = create_quant_regfile_data(
            thermal_scale_mult, thermal_scale_shift, thermal_zero_point,
            power_scale_mult, power_scale_shift, power_zero_point);

        // Write to the appropriate register for this sensor
        *(quant_regfile_base_addr + i) = quant_data;
    }

    // Configure each sensor's dequantization parameters
    for (uint8_t i = 0; i < num_sensors; i++)
    {
        // Example values - replace with actual calibration values
        uint16_t dequant_scale = 0x00C0 + i;
        uint8_t dequant_shift = 6 + i;
        uint8_t dequant_zero = 120 + i;

        uint64_t dequant_data = create_dequant_regfile_data(
            dequant_scale, dequant_shift, dequant_zero);

        // Write to the appropriate register for this sensor
        *(dequant_regfile_base_addr + i) = dequant_data;
    }
}

uint64_t create_quant_regfile_data(
    uint16_t thermal_scale_multiplier,
    uint8_t thermal_scale_shift,
    uint8_t thermal_zero_point,
    uint16_t power_scale_multiplier,
    uint8_t power_scale_shift,
    uint8_t power_zero_point)
{
    uint64_t data = 0;

    // Thermal quantization parameters
    data |= (thermal_scale_multiplier & 0xFFFFULL);  // 16 bits [15:0]
    data |= ((thermal_scale_shift & 0x3FULL) << 16); // 6 bits [21:16]
    data |= ((thermal_zero_point & 0xFFULL) << 22);  // 8 bits [29:22]

    // Power quantization parameters
    data |= ((power_scale_multiplier & 0xFFFFULL) << 32); // 16 bits [47:32]
    data |= ((power_scale_shift & 0x3FULL) << 48);        // 6 bits [53:48]
    data |= ((power_zero_point & 0xFFULL) << 54);         // 8 bits [61:54]

    // Bits [63:62] are reserved and set to 0

    return data;
}

uint64_t create_dequant_regfile_data(
    uint16_t dequant_scale_fixed,
    uint8_t dequant_scale_shift,
    uint8_t dequant_zero_point)
{
    uint64_t data = 0;

    data |= (dequant_scale_fixed & 0xFFFFULL);       // 16 bits [15:0]
    data |= ((dequant_scale_shift & 0x3FULL) << 16); // 6 bits [21:16]
    data |= ((dequant_zero_point & 0xFFULL) << 22);  // 8 bits [29:22]

    // Bits [63:30] are reserved and set to 0

    return data;
}
