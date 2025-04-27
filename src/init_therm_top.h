//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     Header file for therm top start-up configuration
//////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_THERM_TOP_H
#define INIT_THERM_TOP_H

#include <stdint.h>

void init_therm_top(void);
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

#endif // INIT_THERM_TOP_H