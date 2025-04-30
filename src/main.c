//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
// #include "load_npu.h"
#include "init_config.h"
#include "store_data.h"
#include "nn_inference.h"


int main(void)
{
    init_uart(10000000, 101000);
    print_uart("Hello RISC-V CPU!\n");

    init_rl_scheduler();
    init_q_table();
    init_sensor_weight();
    init_standardization_unit();

    // NPU_load();

    init_therm_top();

    print_uart("Config initialized!\n");
    uint64_t i = 0;
    while (1)
    {
        // Store sensor data
        store_sensor_data(i);

        // NN model inference
        run_model_inference();

        // Store prediction data
        store_pred_data(i);
        i++;
    }

    // while (1) {
    //     asm volatile(
    //         "li t5, 0xC0DEDEAD\n"
    //         "nop\n"
    //         );
    // };

    return 0;
}
