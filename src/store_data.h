//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     Header file for data storage via CPU.
//////////////////////////////////////////////////////////////////////////////////

#ifndef STORE_DATA_H
#define STORE_DATA_H

#include <stdint.h>

void store_sensor_data(uint8_t itr, uint8_t read_code, uint8_t read_freq);
void store_pred_data(uint8_t itr);
void store_rl_action(uint8_t itr);
void store_sensor_freq_to_mem(int itr, uint32_t sensor_data_base_addr);
void conv_and_store_sensor_code_to_mem(int itr, uint32_t sensor_data_base_addr);

#endif // STORE_DATA_H