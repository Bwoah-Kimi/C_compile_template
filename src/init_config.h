//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     Header file for constant configuration
//////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_CONFIG_H
#define INIT_CONFIG_H

#include <stdint.h>

#include "address_map.h"
#include "load_npu.h"
#include "nn_inference.h"
#include "store_data.h"

#define NUM_STATES 512
#define ACTIONS_PER_TABLE 4
#define TOTAL_ACTIONS (ACTIONS_PER_TABLE * 2)

#define WEIGHT_T_NUM 6
#define WEIGHT_V_NUM 6
#define WEIGHT_I_NUM 10
#define NUM_SENSORS 14
#define LOG_OUTPUT_WIDTH = 11

void init_testing(void);

void init_therm_top(void);

void init_rl_scheduler(void);

void init_sensor_weight(void);

void init_standardization_unit(void);

void init_q_table(void);

uint64_t create_rl_config_regfile_0_data(
    uint16_t coef_k1,
    uint16_t coef_k2,
    uint16_t coef_k3,
    uint16_t learning_rate);

uint64_t create_rl_config_regfile_1_data(
    uint16_t discount_factor,
    uint16_t initial_epsilon,
    uint8_t num_active_actions,
    uint8_t thres_high,
    uint8_t thres_mid,
    uint8_t thres_low);

uint64_t create_rl_config_regfile_2_data(
    uint16_t epsilon_decay_step,
    uint16_t epsilon_min,
    uint16_t epsilon_decay_factor,
    uint8_t epsilon_decay_interval,
    uint8_t epsilon_decay_mode,
    uint8_t coef_frac_bits);

uint64_t create_top_regfile_0_data(uint8_t therm_top_start,
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
    uint32_t sampling_intvl);

uint64_t create_top_regfile_1_data(uint32_t sensor_data_base_addr,
    uint32_t pred_data_base_addr);

uint64_t create_top_regfile_2_data(uint16_t npu_input_buf_base_addr,
    uint16_t npu_output_buf_base_addr,
    uint16_t synthetic_sensor_thermal_encodings,
    uint16_t synthetic_sensor_current_encodings,
    uint16_t synthetic_sensor_voltage_encodings);
uint64_t create_top_regfile_3_data(uint32_t synthetic_action_sequence, uint8_t store_pred_mode, uint8_t store_action_mode, uint32_t action_base_addr);

#endif // INIT_CONFIG_H