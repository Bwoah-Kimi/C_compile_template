#ifndef CREATE_REGFILE_DATA_H
#define CREATE_REGFILE_DATA_H

#include <stdint.h>

// Function to create the top register file 0 data
uint64_t create_top_regfile_0_data(
    uint8_t therm_top_start,
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
    uint32_t sampling_intvl);

// Function to create the top register file 1 data
uint64_t create_top_regfile_1_data(
    uint16_t action_base_addr,
    uint16_t sensor_data_base_addr,
    uint16_t pred_data_base_addr);

// Function to create the top register file 2 data
uint64_t create_top_regfile_2_data(
    uint16_t npu_input_buf_base_addr,
    uint16_t npu_output_buf_base_addr,
    uint8_t synthetic_sensor_thermal_encodings,
    uint8_t synthetic_sensor_current_encodings,
    uint8_t synthetic_sensor_voltage_encodings);

// Function to create the top register file 3 data
uint64_t create_top_regfile_3_data(
    uint32_t synthetic_action_sequence);

// Function to create the RL configuration register file 0 data
uint64_t create_rl_config_regfile_0_data(
    uint16_t coef_k1,
    uint16_t coef_k2,
    uint16_t coef_k3,
    uint16_t learning_rate);

// Function to create the RL configuration register file 1 data
uint64_t create_rl_config_regfile_1_data(
    uint16_t discount_factor,
    uint16_t initial_epsilon,
    uint8_t num_active_actions,
    uint8_t thres_high,
    uint8_t thres_mid,
    uint8_t thres_low);

// Function to create the RL configuration register file 2 data
uint64_t create_rl_config_regfile_2_data(
    uint16_t epsilon_decay_step,
    uint16_t epsilon_min,
    uint16_t epsilon_decay_factor,
    uint8_t epsilon_decay_interval,
    uint8_t epsilon_decay_mode);

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

#endif // CREATE_REGFILE_DATA_H