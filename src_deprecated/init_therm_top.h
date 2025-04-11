//////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-09
// Description:     Header file for initializing therm_top config regfiles.
//////////////////////////////////////////////////////////////////////////////

#ifndef INIT_THERM_TOP_H
#define INIT_THERM_TOP_H

#include <stdint.h>

void init_therm_top(void);

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

#endif // INIT_THERM_TOP_H