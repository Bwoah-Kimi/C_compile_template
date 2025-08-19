//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu, Xueping Liu
// Acknowledgement: GitHub Copilot
// Description:     C code for IVT sensor testing
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"
#include "store_data.h"

int main(void) {
    // init_uart(526000000, 144000);
    //  print_uart("Starting IVT sensor test...\n");

    init_power_switch();
    init_sensor_weight();

    uint32_t num_itr = 3;
    uint32_t sensor_data_base_addr = 0xA000;
    uint32_t pred_data_base_addr = 0xB000;
    uint32_t action_base_addr = 0xC000;
    init_therm_top(
        1,                     // therm_top_start
        1,                     // therm_top_en
        0,                     // therm_top_stop
        1,                     // collect_en
        1,                     // collect_mode
        0,                     // pred_en
        1,                     // pred_mode
        0,                     // schedule_en
        1,                     // store_sensor_mode
        4,                     // action_offset
        num_itr,               // num_itr
        10000,                 // sampling_intvl
        sensor_data_base_addr, // sensor_data_base_addr
        pred_data_base_addr,   // pred_data_base_addr
        action_base_addr,      // action_base_addr
        0x0,                   // npu_input_buf_base_addr
        0x8,                   // npu_output_buf_base_addr
        0x10,                  // synthetic_sensor_thermal_encodings
        0x3,                   // synthetic_sensor_current_encodings
        0x2,                   // synthetic_sensor_voltage_encodings
        342391,                // synthetic_action_sequence
        0,                     // store_pred_mode
        0                      // store_action_mode
    );

    for (int i = 0; i < num_itr; i++) {
        // store_sensor_data(i, 0, 1);
        // store_sensor_freq_to_mem(i, sensor_data_base_addr);
        conv_and_store_sensor_code_to_mem(i, sensor_data_base_addr);
    }
    // print_uart("IVT sensor test finished!\n");
    return 0;
}