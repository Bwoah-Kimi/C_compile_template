//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     C file for data storage via CPU.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "const_values.h"
#include "store_data.h"
#include "uart.h"

void store_sensor_data(uint8_t itr, uint8_t read_code, uint8_t read_freq) {
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    if (read_code) {
        // Read sensor data from therm top
        // print_uart("IVTOutputCount:");
        // print_uart_byte(itr);
        // print_uart(":\n");
        uint64_t* sensor_data_base_addr = (uint64_t*)SENSOR_ENCODINGS_BASE_ADDR;
        for (int j = 0; j < 14; j++)
        {
            uint64_t sensor_data = *(sensor_data_base_addr + j);

            // Extract individual encodings according to the bit layout
            uint16_t thermal_code = (uint16_t)(sensor_data & 0x3FF);           // [9:0]
            uint16_t voltage_code = (uint16_t)((sensor_data >> 10) & 0x3FF);   // [19:10]
            uint16_t current_code = (uint16_t)((sensor_data >> 20) & 0xFFFF);  // [35:20]
            uint32_t power_code = (uint32_t)((sensor_data >> 36) & 0x3FFFFFF); // [61:36]

            // print_uart("sensor_id ");
            // print_uart_int((uint32_t)j);
            // print_uart(": ");

            // Format: T, V, I, P
            if (j == 13) {
                print_uart_int((uint32_t)thermal_code);
                print_uart(",");
                print_uart_int((uint32_t)voltage_code);
                print_uart(",");
                print_uart_int((uint32_t)current_code);
                print_uart(",");
                print_uart_int(power_code);
                print_uart(",");
            }

        }
        // print_uart("\n");
    }

    if (read_freq) {
        // Read frequency data from therm top
        // print_uart("FreqOutputCount");
        // print_uart_byte(itr);
        // print_uart(":\n");
        uint64_t* freq_data_base_addr = (uint64_t*)SENSOR_FREQ_BUFFER_BASE_ADDR;
        for (int j = 0; j < 14; j++)
        {
            uint64_t freq_data = *(freq_data_base_addr + j);
            // Extract frequency values according to the bit layout
            uint16_t freq_0 = (uint16_t)(freq_data & 0x3FF);           // [9:0] - 10-bit
            uint16_t freq_1 = (uint16_t)((freq_data >> 10) & 0x7FF);   // [20:10] - 11-bit
            uint16_t freq_2 = (uint16_t)((freq_data >> 21) & 0x7FF);   // [31:21] - 11-bit

            // print_uart("sensor_addr");
            // print_uart_int((uint32_t)j);
            // print_uart(": ");

            if (j == 13) {
                print_uart_int((uint32_t)freq_0);
                print_uart(",");
                print_uart_int((uint32_t)freq_1);
                print_uart(",");
                print_uart_int((uint32_t)freq_2);
                print_uart(",\n");
            }
        }
        // print_uart("\n");
    }

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG;
    return;
}


void store_pred_data(uint8_t itr) {
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_PRED_START_FLAG) {}

    // Read prediction data from therm top
    print_uart("PRED ITR ");
    print_uart_byte(itr);
    print_uart(":\n");
    uint64_t* pred_data_base_addr = (uint64_t*)SENSOR_PRED_BASE_ADDR; // Base address of prediction data
    for (int j = 0; j < 14; j++)
    {
        uint64_t pred_data = *(pred_data_base_addr + j); // Read prediction data
        print_uart("IDX ");
        print_uart_byte(j);
        print_uart(": ");
        print_uart_addr(pred_data); // Print prediction data
        print_uart("\n");
    }
    print_uart("\n");

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_PRED_FINISH_FLAG; // Set finish flag
}

void store_rl_action(uint8_t itr) {
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_ACTION_START_FLAG) {}

    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_ACTION_FINISH_FLAG; // Set finish flag
}
