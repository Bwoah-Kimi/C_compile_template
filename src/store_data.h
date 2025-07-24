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

void verify_sram_data(volatile uint64_t* sram_data, const uint64_t* expected_data, uint64_t count, uint64_t flag_value);
#endif // STORE_DATA_H