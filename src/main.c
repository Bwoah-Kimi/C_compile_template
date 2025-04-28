//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "uart.h"
#include "init_config.h"
#include "init_therm_top.h"
#include "store_data.h"
#include "nn_inference.h"

int main(void)
{
    init_uart(10000000, 101000); // Initialize UART with 500MHz clock and 115200 baud rate
    print_uart("Hello RISC-V CPU!\n"); // Print a message to UART
    init_config();
    init_therm_top(); // Initialize the thermal top configuration

    for (uint8_t i = 0; i < 10; i++)
    {
        // Store sensor data
        store_sensor_data(i);

        // NN model inference
        run_model_inference();

        // Store prediction data
        store_pred_data(i);
    }
    return 0;
}
