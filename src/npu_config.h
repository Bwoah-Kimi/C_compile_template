//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-08-22
// Description:     Header file for NPU configuration.
//////////////////////////////////////////////////////////////////////////////////

#ifndef NPU_CONFIG_H
#define NPU_CONFIG_H

#include <stdint.h>

void init_cim_config();
void init_macro_data();
void change_npu_task(uint8_t func_id, bool end_compute);
void change_input_data(uint8_t data_id, int start_idx, int end_idx);
void change_med_data(uint8_t data_id, int start_idx, int end_idx);
void change_macro_data(uint8_t data_id, uint8_t macro_id);
void read_macro_data();
void macro_select(uint8_t macro_id, bool end_compute);
void npu_all_pwr(bool end_compute);
void npu_func0_pwr(bool end_compute);
void npu_func1_pwr(bool end_compute);
void npu_func2_pwr(bool end_compute);
void sync_npu(bool change_task, uint8_t task_id, bool end_compute, uint32_t itr, uint32_t sensor_data_base_addr);
void change_npu_task(uint8_t task_id, bool end_compute);
void change_npu_func(uint8_t func_id);
#endif // NPU_CONFIG_H