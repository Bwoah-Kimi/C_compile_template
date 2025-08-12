//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu, Xueping Liu
// Acknowledgement: GitHub Copilot
// Description:     C code for IVT sensor testing
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"

// Power switch and sensor weight values are placed in memory by linker script
//  set{int}0x20000000=
//  set{int}0x20000004=
//  set{int}0x2000000c=
//  Freqref=7.2MHz

int main(void) {
    //  *(uint32_t*)DCO_CONFIG_BASE_ADDR = 22;        // FC
    //  *(uint32_t*)(DCO_CONFIG_BASE_ADDR + 1) = 10;  // CC
    //  *(uint32_t*)(DCO_CONFIG_BASE_ADDR + 3) = 0;   // DIV

    init_uart(526000000, 144000); // Hz
    //  print_uart("Starting IVT sensor test...\n");

    init_power_switch();
    init_sensor_weight();

    uint32_t num_itr = 3;
    init_therm_top(
        1,        // therm_top_start
        1,        // therm_top_en
        0,        // therm_top_stop
        1,        // collect_en
        1,        // collect_mode
        0,        // pred_en
        1,        // pred_mode
        0,        // schedule_en
        1,        // store_sensor_mode
        4,        // action_offset
        num_itr,  // num_itr
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

    for (int i = 0; i < num_itr; i++) {
        store_sensor_data(
            i,
            0,  // read_code
            1   // read_freq
        );
    }
    // print_uart("IVT sensor test finished!\n");
    return 0;
}