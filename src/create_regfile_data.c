#include <stdint.h>
#include "../include/create_regfile_data.h"

// Create CSR data for Configuration Register 0
uint64_t create_top_regfile_0_data(uint8_t therm_top_start,
                                   uint8_t therm_top_en,
                                   uint8_t therm_top_stop,
                                   uint8_t collect_en,
                                   uint8_t collect_mode,
                                   uint8_t pred_en,
                                   uint8_t schedule_en,
                                   uint8_t store_sensor_en,
                                   uint8_t store_pred_en,
                                   uint8_t store_action_en,
                                   uint8_t action_offset,
                                   uint32_t num_itr,
                                   uint32_t sampling_intvl)
{
    uint64_t data = 0;

    // Single bit fields
    data |= (therm_top_start & 0x1ULL);
    data |= ((therm_top_en & 0x1ULL) << 1);
    data |= ((therm_top_stop & 0x1ULL) << 2);
    data |= ((collect_en & 0x1ULL) << 3);
    data |= ((collect_mode & 0x1ULL) << 4);
    data |= ((pred_en & 0x1ULL) << 5);
    data |= ((schedule_en & 0x1ULL) << 6);
    data |= ((store_sensor_en & 0x1ULL) << 7);
    data |= ((store_pred_en & 0x1ULL) << 8);
    data |= ((store_action_en & 0x1ULL) << 9);

    // Multi-bit fields
    data |= ((action_offset & 0x1FULL) << 10); // 5 bits [14:10]
    data |= ((num_itr & 0x1FFFFULL) << 15);    // 17 bits [31:15]
    data |=
        ((uint64_t)(sampling_intvl & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]

    return data;
}

uint64_t create_top_regfile_1_data(uint16_t action_base_addr,
                                   uint16_t sensor_data_base_addr,
                                   uint16_t pred_data_base_addr)
{
    uint64_t data = 0;
    data |= (action_base_addr & 0xFFFFULL);              // 16 bits [15:0]
    data |= ((sensor_data_base_addr & 0xFFFFULL) << 16); // 16 bits [31:16]
    data |= ((pred_data_base_addr & 0xFFFFULL) << 32);   // 16 bits [47:32]

    // Bits [63:48] are reserved and set to 0
    return data;
}

uint64_t create_top_regfile_2_data(uint16_t npu_input_buf_base_addr,
                                   uint16_t npu_output_buf_base_addr,
                                   uint8_t synthetic_sensor_thermal_encodings,
                                   uint8_t synthetic_sensor_current_encodings,
                                   uint8_t synthetic_sensor_voltage_encodings)
{
    uint64_t data = 0;
    data |= (npu_input_buf_base_addr & 0xAULL);          // 10 bits [9:0]
    data |= ((npu_output_buf_base_addr & 0xAULL) << 10); // 9 bits [19:10]
    // Bits [29:20] are reserved and set to 0
    data |= ((synthetic_sensor_thermal_encodings & 0xFFULL) << 32); // 8 bits [39:32]
    data |= ((synthetic_sensor_current_encodings & 0xFFULL) << 40); // 8 bits [47:40]
    data |= ((synthetic_sensor_voltage_encodings & 0xFFULL) << 48); // 8 bits [55:48]
    // Bits [63:56] are reserved and set to 0

    return data;
}

uint64_t create_top_regfile_3_data(uint32_t synthetic_action_sequence)
{
    uint64_t data = 0;
    data |= (synthetic_action_sequence & 0xFFFFFFFFULL); // 32 bits [31:0]
    // Bits [63:32] are reserved and set to 0
    return data;
}

// Create CSR data for RL Configuration Register 0
uint64_t create_rl_config_regfile_0_data(
    uint16_t coef_k1,
    uint16_t coef_k2,
    uint16_t coef_k3,
    uint16_t learning_rate)
{
    uint64_t data = 0;

    data |= (coef_k1 & 0xFFFFULL);               // 16 bits [15:0]
    data |= ((coef_k2 & 0xFFFFULL) << 16);       // 16 bits [31:16]
    data |= ((coef_k3 & 0xFFFFULL) << 32);       // 16 bits [47:32]
    data |= ((learning_rate & 0xFFFFULL) << 48); // 16 bits [63:48]

    return data;
}

// Create CSR data for RL Configuration Register 1
uint64_t create_rl_config_regfile_1_data(
    uint16_t discount_factor,
    uint16_t initial_epsilon,
    uint8_t num_active_actions,
    uint8_t thres_high,
    uint8_t thres_mid,
    uint8_t thres_low)
{
    uint64_t data = 0;

    data |= (discount_factor & 0xFFFFULL);          // 16 bits [15:0]
    data |= ((initial_epsilon & 0xFFFFULL) << 16);  // 16 bits [31:16]
    data |= ((num_active_actions & 0xFFULL) << 32); // 8 bits [39:32]
    data |= ((thres_high & 0xFFULL) << 40);         // 8 bits [47:40]
    data |= ((thres_mid & 0xFFULL) << 48);          // 8 bits [55:48]
    data |= ((thres_low & 0xFFULL) << 56);          // 8 bits [63:56]

    return data;
}

// Create CSR data for RL Configuration Register 2
uint64_t create_rl_config_regfile_2_data(
    uint16_t epsilon_decay_step,
    uint16_t epsilon_min,
    uint16_t epsilon_decay_factor,
    uint8_t epsilon_decay_interval,
    uint8_t epsilon_decay_mode)
{
    uint64_t data = 0;

    data |= (epsilon_decay_step & 0xFFFFULL);           // 16 bits [15:0]
    data |= ((epsilon_min & 0xFFFFULL) << 16);          // 16 bits [31:16]
    data |= ((epsilon_decay_factor & 0xFFFFULL) << 32); // 16 bits [47:32]
    data |= ((epsilon_decay_interval & 0xFFULL) << 48); // 8 bits [55:48]
    data |= ((epsilon_decay_mode & 0x3ULL) << 56);      // 2 bits [57:56]

    return data;
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