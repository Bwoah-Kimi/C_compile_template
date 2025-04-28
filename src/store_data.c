//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     C file for data storage via CPU.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "store_data.h"
#include "uart.h"

void store_sensor_data(uint8_t itr)
{
    // Wait for the CPU start flag
    while ((*(uint64_t*)0x60007000) != 0x00000000B1B1B1B1)
    {
        // Do nothing, just wait
    }
    // Read sensor data from therm top
    print_uart("SENSOR ITR ");
    print_uart_byte(itr);
    print_uart(":\n");
    uint64_t* sensor_data_base_addr = (uint64_t*)0x60005000; // Base address of sensor data
    for (int j = 0; j < 14; j++)
    {
        uint64_t sensor_data = *(sensor_data_base_addr + j); // Read sensor data
        print_uart("IDX ");
        print_uart_byte(j);
        print_uart(": ");
        print_uart_addr(sensor_data); // Print sensor data
        print_uart("\n");
    }
    // Set finish flag
    *(volatile uint64_t*)0x60007008 = 0x1B1B1B1B00000000; // Set finish flag
    return;
}

void store_pred_data(uint8_t itr)
{
    while ((*(uint64_t*)0x60007000) != 0x00000000C2C2C2C2)
    {
        // Do nothing, just wait
    }
    // Read prediction data from therm top
    print_uart("PRED ITR ");
    print_uart_byte(itr);
    print_uart(":\n");
    uint64_t* pred_data_base_addr = (uint64_t*)0x60006000; // Base address of prediction data
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
    *(volatile uint64_t*)0x60007008 = 0x2C2C2C2C00000000; // Set finish flag
}

void store_rl_action(uint8_t itr)
{
    while ((*(uint64_t*)0x60007000) != 0x00000000D3D3D3D3)
    {
        // Do nothing, just wait
    }
}
