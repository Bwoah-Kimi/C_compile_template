//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "uart.h"
#include "init_config.h"
#include "init_therm_top.h"

int main(void)
{
    init_uart(500000000, 115200); // Initialize UART with 500MHz clock and 115200 baud rate
    init_config();
    init_therm_top(); // Initialize the thermal top configuration

    for (int i = 0; i < 10; i++)
    {
        // Wait for the CPU start flag
        while ((*(volatile uint64_t *)0x60007000) != 0x00000000B1B1B1B1)
        {
            // Do nothing, just wait
        }
        // Read sensor data from therm top
        print_uart("Sensor data iteration ");
        print_uart_int(i);
        print_uart(":\n");
        uint64_t *sensor_data_base_addr = (uint64_t *)0x60005000; // Base address of sensor data
        for (int j = 0; j < 14; j++)
        {
            uint64_t sensor_data = *(sensor_data_base_addr + j); // Read sensor data
            print_uart("Sensor ");
            print_uart_int(j);
            print_uart(": ");
            print_uart_addr(sensor_data); // Print sensor data
            print_uart("\n");
        }
        // Set finish flag
        *(volatile uint64_t *)0x60007008 = 0x1B1B1B1B00000000; // Set finish flag
    }
    return 0;
}
