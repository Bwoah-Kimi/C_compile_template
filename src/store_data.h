//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     Header file for data storage via CPU.
//////////////////////////////////////////////////////////////////////////////////

#ifndef STORE_DATA_H
#define STORE_DATA_H

#include <stdint.h>

void store_sensor_data(uint8_t itr);
void store_pred_data(uint8_t itr);

#endif // STORE_DATA_H