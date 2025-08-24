//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu, Mingxuan Li
// Acknowledgement: GitHub Copilot
// Description:     C code for IVT sensor testing
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "init_config.h"
#include "const_values.h"
#include "d2pu_config.h"
#include "store_data.h"
#include "npu_config.h"

static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}


static inline uint64_t read_hw_cycles(void) {
    return *(volatile uint64_t*)0x60008008;
}


void measure_and_compare_cycles(int iteration, uint8_t read_code, uint8_t read_freq) {
    // Start cycle measurement - both methods
    uint64_t start_cycles = read_cycles();
    uint64_t start_hw_cycles = read_hw_cycles();

    store_sensor_data(
        iteration,
        read_code,
        read_freq
    );

    // End cycle measurement - both methods
    uint64_t end_cycles = read_cycles();
    uint64_t end_hw_cycles = read_hw_cycles();

    uint64_t cycles_taken = end_cycles - start_cycles;
    uint64_t hw_cycles_taken = end_hw_cycles - start_hw_cycles;

    print_uart("Iteration ");
    print_uart_int(iteration);
    print_uart(":\n");
    print_uart("  CPU cycles: ");
    print_uart_addr(cycles_taken);  // Print as hex
    print_uart(" (");
    print_uart_dec(cycles_taken);   // Print as decimal
    print_uart(")\n");
    print_uart("  HW cycles:  ");
    print_uart_addr(hw_cycles_taken);  // Print as hex
    print_uart(" (");
    print_uart_dec(hw_cycles_taken);   // Print as decimal
    print_uart(")\n");

    // Calculate difference
    int64_t diff = (int64_t)cycles_taken - (int64_t)hw_cycles_taken;
    print_uart("  Difference: ");
    if (diff >= 0) {
        print_uart_dec((uint64_t)diff);
    }
    else {
        print_uart("-");
        print_uart_dec((uint64_t)(-diff));
    }
    print_uart(" cycles\n\n");
}


int main() {
    init_uart(980000000, 256000);

    print_uart("================================================\n");
    print_uart("Starting Thermal Profiling Test\n");
    print_uart("================================================\n");

    init_power_switch();
    init_sensor_weight();
    init_cim_config();
    init_macro_data();

    // change_med_data(3);
    // change_input_data(4);
    // change_macro_data(2, 2);

    // read_macro_data();

    change_npu_task(11, false);

    // npu_all_pwr(false);
    // npu_func0_pwr(false);
    // npu_func1_pwr(false);
    // npu_func2_pwr(false);
    // macro_select(1, false);

    uint32_t power_cycle = 1;
    uint32_t num_itr = 100;
    uint32_t curr_itr = 0;
    uint32_t sampling_intvl = 10000;
    uint8_t store_sensor_mode = 3; // 2'b00 Don't store; 2'b01 UART; 2'b10 Main memory; 2'b11 Both
    uint32_t sensor_data_base_addr = 0xA000;
    uint32_t pred_data_base_addr = 0xB000;
    uint32_t action_base_addr = 0xC000;

    init_therm_top(
        1,                      // therm_top_start
        1,                      // therm_top_en
        0,                      // therm_top_stop
        1,                      // collect_en
        1,                      // collect_mode
        0,                      // pred_en
        1,                      // pred_mode
        0,                      // schedule_en
        store_sensor_mode,      // store_sensor_mode
        0,                      // action_offset
        num_itr,                // num_itr
        sampling_intvl,         // sampling_intvl
        sensor_data_base_addr,  // sensor_data_base_addr
        pred_data_base_addr,    // pred_data_base_addr
        action_base_addr,       // action_base_addr
        0x0,                    // npu_input_buf_base_addr
        0x8,                    // npu_output_buf_base_addr
        0x10,                   // synthetic_sensor_thermal_encodings
        0x3,                    // synthetic_sensor_current_encodings
        0x2,                    // synthetic_sensor_voltage_encodings
        342391,                 // synthetic_action_sequence
        0,                      // store_pred_mode
        0                       // store_action_mode
    );

    for (uint32_t j = 0; j < power_cycle; j++) {
        if (store_sensor_mode == 1) {
            macro_select(9, true);
            // npu_func0_pwr(true);
            for (uint32_t i = 0; i < num_itr / power_cycle / 2; i++) {
                // measure_and_compare_cycles(i, 1, 1);
                store_sensor_data(
                    curr_itr,
                    1,  // read_code
                    1   // read_freq
                );
                curr_itr++;
            }
            macro_select(9, false);
            // npu_func0_pwr(false);
            for (uint32_t i = 0; i < num_itr / power_cycle / 2; i++) {
                // measure_and_compare_cycles(i, 1, 1);
                store_sensor_data(
                    curr_itr,
                    1,  // read_code
                    1   // read_freq
                );
                curr_itr++;
            }
        }
        else if (store_sensor_mode == 3) {
            sync_npu(false, 0, true, curr_itr, sensor_data_base_addr);
            for (curr_itr = 1; curr_itr < 20; curr_itr++) {
                sync_npu(false, 0, true, curr_itr, sensor_data_base_addr);
            }
            sync_npu(false, 0, false, curr_itr, sensor_data_base_addr);
            for (curr_itr = 21; curr_itr < 100; curr_itr++) {
                sync_npu(false, 0, false, curr_itr, sensor_data_base_addr);
            }
            // for (uint32_t i = 0; i < 100; i++) {
            //     conv_and_store_sensor_code_to_mem(i, sensor_data_base_addr);
            // }
        }
    }

    // macro_select(11, true);
    // npu_all_pwr(true);
    // npu_func0_pwr(true);
    // npu_func1_pwr(true);
    // npu_func2_pwr(true);

    return 0;
}