//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-30
// Description:     C Code to Initialize config regfiles.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "const_values.h"
#include "init_config.h"
#include "uart.h"

void init_therm_top(
    uint8_t therm_top_start,
    uint8_t therm_top_en,
    uint8_t therm_top_stop,
    uint8_t collect_en,
    uint8_t collect_mode,
    uint8_t pred_en,
    uint8_t pred_mode,
    uint8_t schedule_en,
    uint8_t store_sensor_mode,
    uint8_t action_offset,
    uint32_t num_itr,
    uint32_t sampling_intvl,
    uint32_t sensor_data_base_addr,
    uint32_t pred_data_base_addr,
    uint32_t action_base_addr,
    uint16_t npu_input_buf_base_addr,
    uint16_t npu_output_buf_base_addr,
    uint16_t synthetic_sensor_thermal_encodings,
    uint16_t synthetic_sensor_current_encodings,
    uint16_t synthetic_sensor_voltage_encodings,
    uint32_t synthetic_action_sequence,
    uint8_t store_pred_mode,
    uint8_t store_action_mode
) {
    uint64_t top_regfile_0_data = create_top_regfile_0_data(therm_top_start, therm_top_en, therm_top_stop, collect_en, collect_mode, pred_en, pred_mode, schedule_en, store_sensor_mode, action_offset, num_itr, sampling_intvl);
    uint64_t top_regfile_1_data = create_top_regfile_1_data(sensor_data_base_addr, pred_data_base_addr);
    uint64_t top_regfile_2_data = create_top_regfile_2_data(npu_input_buf_base_addr, npu_output_buf_base_addr, synthetic_sensor_thermal_encodings, synthetic_sensor_current_encodings, synthetic_sensor_voltage_encodings);
    uint64_t top_regfile_3_data = create_top_regfile_3_data(synthetic_action_sequence, store_pred_mode, store_action_mode, action_base_addr);

    // Write the data to respective registers
    uint64_t* top_regfile_base_addr = (uint64_t*)TOP_CONFIG_REGFILE_BASE_ADDR; // Base address of top register file
    *(top_regfile_base_addr + 1) = top_regfile_1_data;        // Write to regfile 1
    *(top_regfile_base_addr + 2) = top_regfile_2_data;        // Write to regfile 2
    *(top_regfile_base_addr + 3) = top_regfile_3_data;        // Write to regfile 3
    *(top_regfile_base_addr + 0) = top_regfile_0_data;        // Write to regfile 0
    return;
}

void init_rl_scheduler(void) {
    // RL config regfile 0
    uint16_t coef_k1 = 0x1000;
    uint16_t coef_k2 = 0x0800;
    uint16_t coef_k3 = 0x0400;
    uint16_t learning_rate = 0x0100;

    // RL config regfile 1
    uint16_t discount_factor = 0x0200;
    uint16_t initial_epsilon = 0x0F00;
    uint8_t num_active_actions = 8;
    uint8_t thres_high = 0x50;
    uint8_t thres_mid = 0x30;
    uint8_t thres_low = 0x10;

    // RL config regfile 2
    uint16_t epsilon_decay_step = 0x00CC;
    uint16_t epsilon_min = 0x0005;
    uint16_t epsilon_decay_factor = 0x000A;
    uint8_t epsilon_decay_interval = 0x10;
    uint8_t epsilon_decay_mode = 0;
    uint8_t coef_frac_bits = 12;

    // Create register data
    uint64_t rl_regfile_0_data = create_rl_config_regfile_0_data(coef_k1, coef_k2, coef_k3, learning_rate);
    uint64_t rl_regfile_1_data = create_rl_config_regfile_1_data(discount_factor, initial_epsilon, num_active_actions, thres_high, thres_mid, thres_low);
    uint64_t rl_regfile_2_data = create_rl_config_regfile_2_data(epsilon_decay_step, epsilon_min, epsilon_decay_factor, epsilon_decay_interval, epsilon_decay_mode, coef_frac_bits);

    // Write the data to respective registers
    uint64_t* rl_config_regfile_base = (uint64_t*)RL_CONFIG_REGFILE_BASE_ADDR;
    rl_config_regfile_base[0] = rl_regfile_0_data; // Write to RL regfile 0
    rl_config_regfile_base[1] = rl_regfile_1_data; // Write to RL regfile 1
    rl_config_regfile_base[2] = rl_regfile_2_data; // Write to RL regfile 2
    return;
}

void init_q_table(void) {
    uint64_t* q_table_0_base = (uint64_t*)Q_TABLE_0_BASE_ADDR;
    uint64_t* q_table_1_base = (uint64_t*)Q_TABLE_1_BASE_ADDR;

    for (int i = 0; i < NUM_STATES; i++)
    {
        uint64_t q_values_0 = 0x0;
        uint64_t q_values_1 = 0x0;
        // Write values to Q-tables
        q_table_0_base[i] = q_values_0;
        q_table_1_base[i] = q_values_1;
    }
}

// void init_sensor_weight(void) {
//     int64_t* sensor_weight_buffer_base_addr = (int64_t*)SENSOR_WEIGHT_BUFFER_BASE_ADDR;
//     for (int i = 0; i < WEIGHT_I_NUM + WEIGHT_T_NUM + WEIGHT_V_NUM; i++) {
//         // Write values to sensor weight buffer
//         *(sensor_weight_buffer_base_addr + i) = sensor_weight[i];
//     }
//     return;
// }

// void init_power_switch(void) {
//     uint64_t* power_switch_base_addr = (uint64_t*)POWER_SWITCH_BASE_ADDR;
//     uint64_t reg0 = 0;
//     for (int i = 0; i < 7; i++) {
//         reg0 |= ((uint64_t)power_switch_config[i] << (i * 8)); // Shift each switch config into the correct position
//     }
//     uint64_t reg1 = 0;
//     for (int i = 0; i < 7; i++) {
//         reg1 |= ((uint64_t)power_switch_config[i + 7] << (i * 8)); // Shift each switch config into the correct position
//     }
//     *(power_switch_base_addr + 0) = reg0;
//     *(power_switch_base_addr + 1) = reg1;
//     return;
// }

void init_standardization_unit(void) {
    uint64_t* quant_config_regfile_base_addr = (uint64_t*)QUANT_CONFIG_REGFILE_BASE_ADDR;
    uint64_t* dequant_config_regfile_base_addr = (uint64_t*)DEQUANT_CONFIG_REGFILE_BASE_ADDR;

    // Quantization config
    static uint64_t quant_config[16] = {
        0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
        0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8
    };
    static uint64_t dequant_config[16] = {
        0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
        0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8
    };

    for (int i = 0; i < NUM_SENSORS; i++) {
        // Write values to quantization config regfile
        quant_config_regfile_base_addr[i] = quant_config[i];
        dequant_config_regfile_base_addr[i] = dequant_config[i];
    }
}

void init_npu(void) {
    return;
}

uint64_t create_top_regfile_0_data(uint8_t therm_top_start, uint8_t therm_top_en, uint8_t therm_top_stop, uint8_t collect_en, uint8_t collect_mode, uint8_t pred_en, uint8_t pred_mode, uint8_t schedule_en, uint8_t store_sensor_mode, uint8_t action_offset, uint32_t num_itr, uint32_t sampling_intvl) {
    uint64_t data = 0;
    data |= (therm_top_start & 0x1ULL);
    data |= ((therm_top_en & 0x1ULL) << 1);
    data |= ((therm_top_stop & 0x1ULL) << 2);
    data |= ((collect_en & 0x1ULL) << 3);
    data |= ((collect_mode & 0x1ULL) << 4);
    data |= ((pred_en & 0x1ULL) << 5);
    data |= ((pred_mode & 0x1ULL) << 6);
    data |= ((schedule_en & 0x1ULL) << 7);
    data |= ((store_sensor_mode & 0x3ULL) << 8);
    data |= ((action_offset & 0x1FULL) << 10); // 5 bits [14:10]
    data |= ((num_itr & 0x1FFFFULL) << 15);    // 17 bits [31:15]
    data |=
        ((uint64_t)(sampling_intvl & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]

    return data;
}

uint64_t create_top_regfile_1_data(uint32_t sensor_data_base_addr, uint32_t pred_data_base_addr) {
    uint64_t data = 0;
    data |= (sensor_data_base_addr & 0xFFFFFFFFULL);       // 32 bits [31:0]
    data |= ((pred_data_base_addr & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]

    return data;
}

uint64_t create_top_regfile_2_data(uint16_t npu_input_buf_base_addr, uint16_t npu_output_buf_base_addr, uint16_t synthetic_sensor_thermal_encodings, uint16_t synthetic_sensor_current_encodings, uint16_t synthetic_sensor_voltage_encodings) {
    uint64_t data = 0;
    data |= (npu_input_buf_base_addr & 0x3FFULL);                     // 10 bits [9:0]
    data |= ((npu_output_buf_base_addr & 0x3FFULL) << 10);            // 10 bits [19:10]
    data |= ((synthetic_sensor_thermal_encodings & 0x3FFULL) << 20);  // 10 bits [29:20]
    data |= ((synthetic_sensor_voltage_encodings & 0x3FFULL) << 30);  // 10 bits [39:30]
    data |= ((synthetic_sensor_current_encodings & 0xFFFFULL) << 40); // 16 bits [55:40]
    // Bits [63:56] are reserved and set to 0
    return data;
}

uint64_t create_top_regfile_3_data(uint32_t synthetic_action_sequence, uint8_t store_pred_mode, uint8_t store_action_mode, uint32_t action_base_addr) {
    uint64_t data = 0;
    data |= (synthetic_action_sequence & 0xFFFFFFULL); // 24 bits [23:0]
    data |= ((store_pred_mode & 0x3ULL) << 24);        // 2 bits [25:24]
    data |= ((store_action_mode & 0x3ULL) << 26);      // 2 bits [27:26]
    // Bits [31:28] are reserved and set to 0
    data |= ((action_base_addr & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]
    return data;
}

uint64_t create_rl_config_regfile_0_data(uint16_t coef_k1, uint16_t coef_k2, uint16_t coef_k3, uint16_t learning_rate) {
    uint64_t data = 0;
    data |= (coef_k1 & 0xFFFFULL);               // 16 bits [15:0]
    data |= ((coef_k2 & 0xFFFFULL) << 16);       // 16 bits [31:16]
    data |= ((coef_k3 & 0xFFFFULL) << 32);       // 16 bits [47:32]
    data |= ((learning_rate & 0xFFFFULL) << 48); // 16 bits [63:48]
    return data;
}

uint64_t create_rl_config_regfile_1_data(uint16_t discount_factor, uint16_t initial_epsilon, uint8_t num_active_actions, uint8_t thres_high, uint8_t thres_mid, uint8_t thres_low) {
    uint64_t data = 0;
    data |= (discount_factor & 0xFFFFULL);          // 16 bits [15:0]
    data |= ((initial_epsilon & 0xFFFFULL) << 16);  // 16 bits [31:16]
    data |= ((num_active_actions & 0xFFULL) << 32); // 8 bits [39:32]
    data |= ((thres_high & 0xFFULL) << 40);         // 8 bits [47:40]
    data |= ((thres_mid & 0xFFULL) << 48);          // 8 bits [55:48]
    data |= ((thres_low & 0xFFULL) << 56);          // 8 bits [63:56]    
    return data;
}

uint64_t create_rl_config_regfile_2_data(uint16_t epsilon_decay_step, uint16_t epsilon_min, uint16_t epsilon_decay_factor, uint8_t epsilon_decay_interval, uint8_t epsilon_decay_mode, uint8_t coef_frac_bits)
{
    uint64_t data = 0;
    data |= (epsilon_decay_step & 0xFFFFULL);           // 16 bits [15:0]
    data |= ((epsilon_min & 0xFFFFULL) << 16);          // 16 bits [31:16]
    data |= ((epsilon_decay_factor & 0xFFFFULL) << 32); // 16 bits [47:32]
    data |= ((epsilon_decay_interval & 0xFFULL) << 48); // 8 bits [55:48]
    data |= ((epsilon_decay_mode & 0x1ULL) << 56);      // 1 bits [56]
    data |= ((coef_frac_bits & 0xFULL) << 57);          // 4 bits [60:57]
    return data;
}