//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     C file for therm top start-up configuration
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "init_therm_top.h"

void init_therm_top(void)
{
    // Top config regfile 0
    uint8_t therm_top_start = 1;
    uint8_t therm_top_en = 1;
    uint8_t therm_top_stop = 0;
    uint8_t collect_en = 1;
    uint8_t collect_mode = 1;
    uint8_t pred_en = 0;
    uint8_t pred_mode = 0;
    uint8_t schedule_en = 0;
    uint8_t store_sensor_mode = 3;
    uint8_t action_offset = 4;
    uint32_t num_itr = 10;
    uint32_t sampling_intvl = 200;

    // Top config regfile 1
    uint32_t sensor_data_base_addr = 0x1000;
    uint32_t pred_data_base_addr = 0x2000;
    uint32_t action_base_addr = 0x2800;

    // Top config regfile 2
    uint16_t npu_input_buf_base_addr = 0x10;
    uint16_t npu_output_buf_base_addr = 0x5;

    uint16_t synthetic_sensor_thermal_encodings = 10;
    uint16_t synthetic_sensor_current_encodings = 20;
    uint16_t synthetic_sensor_voltage_encodings = 30;

    // Top config regfile 3
    uint32_t synthetic_action_sequence = 342391;
    uint8_t store_pred_mode = 0;
    uint8_t store_action_mode = 0;

    uint64_t top_regfile_0_data = create_top_regfile_0_data(
        therm_top_start, therm_top_en, therm_top_stop, collect_en,
        collect_mode, pred_en, pred_mode, schedule_en, store_sensor_mode,
        action_offset, num_itr, sampling_intvl);

    uint64_t top_regfile_1_data = create_top_regfile_1_data(sensor_data_base_addr, pred_data_base_addr);

    uint64_t top_regfile_2_data = create_top_regfile_2_data(
        npu_input_buf_base_addr, npu_output_buf_base_addr,
        synthetic_sensor_thermal_encodings,
        synthetic_sensor_current_encodings,
        synthetic_sensor_voltage_encodings);

    uint64_t top_regfile_3_data = create_top_regfile_3_data(synthetic_action_sequence, store_pred_mode, store_action_mode, action_base_addr);

    // Write the data to respective registers

    uint64_t *top_regfile_base_addr = (uint64_t *)0x60002218; // Base address of top register file
    *(top_regfile_base_addr + 1) = top_regfile_1_data;        // Write to regfile 1
    *(top_regfile_base_addr + 2) = top_regfile_2_data;        // Write to regfile 2
    *(top_regfile_base_addr + 3) = top_regfile_3_data;        // Write to regfile 3
    *(top_regfile_base_addr + 0) = top_regfile_0_data;        // Write to regfile 0
}

// Create CSR data for Configuration Register 0
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
                                   uint32_t sampling_intvl)
{
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

uint64_t create_top_regfile_1_data(uint32_t sensor_data_base_addr, uint32_t pred_data_base_addr)
{
    uint64_t data = 0;
    data |= (sensor_data_base_addr & 0xFFFFFFFFULL);       // 32 bits [31:0]
    data |= ((pred_data_base_addr & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]

    return data;
}

uint64_t create_top_regfile_2_data(uint16_t npu_input_buf_base_addr,
                                   uint16_t npu_output_buf_base_addr,
                                   uint16_t synthetic_sensor_thermal_encodings,
                                   uint16_t synthetic_sensor_current_encodings,
                                   uint16_t synthetic_sensor_voltage_encodings)
{
    uint64_t data = 0;
    data |= (npu_input_buf_base_addr & 0x3FFULL);                     // 10 bits [9:0]
    data |= ((npu_output_buf_base_addr & 0x3FFULL) << 10);            // 10 bits [19:10]
    data |= ((synthetic_sensor_thermal_encodings & 0x3FFULL) << 20);  // 10 bits [29:20]
    data |= ((synthetic_sensor_voltage_encodings & 0x3FFULL) << 30);  // 10 bits [39:30]
    data |= ((synthetic_sensor_current_encodings & 0xFFFFULL) << 40); // 16 bits [55:40]
    // Bits [63:56] are reserved and set to 0

    return data;
}

uint64_t create_top_regfile_3_data(uint32_t synthetic_action_sequence,
                                   uint8_t store_pred_mode,
                                   uint8_t store_action_mode,
                                   uint32_t action_base_addr)

{
    uint64_t data = 0;
    data |= (synthetic_action_sequence & 0xFFFFFFULL); // 24 bits [23:0]
    data |= ((store_pred_mode & 0x3ULL) << 24);        // 2 bits [25:24]
    data |= ((store_action_mode & 0x3ULL) << 26);      // 2 bits [27:26]
    // Bits [31:28] are reserved and set to 0
    data |= ((action_base_addr & 0xFFFFFFFFULL) << 32); // 32 bits [63:32]

    return data;
}
