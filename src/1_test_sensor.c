//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Test code for IVT sensor testing
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"

int main(void) {
    init_uart(10000000, 101000);
    print_uart("Starting IVT sensor test...\n");

    init_power_switch();

    init_therm_top(
        1,        // therm_top_start
        1,        // therm_top_en
        0,        // therm_top_stop
        1,        // collect_en
        1,        // collect_mode
        0,        // pred_en
        1,        // pred_mode
        0,        // schedule_en
        2,        // store_sensor_mode
        4,        // action_offset
        1,        // num_itr
        10000,    // sampling_intvl
        0xA000,   // sensor_data_base_addr
        0xB000,   // pred_data_base_addr
        0xC000,   // action_base_addr
        0x0,      // npu_input_buf_base_addr
        0x8,      // npu_output_buf_base_addr
        0x10,     // synthetic_sensor_thermal_encodings
        0x3,      // synthetic_sensor_current_encodings
        0x2,      // synthetic_sensor_voltage_encodings
        342391,   // synthetic_action_sequence
        0,        // store_pred_mode
        0         // store_action_mode
    );

    print_uart("IVT sensor test finished!\n");
    return 0;
}