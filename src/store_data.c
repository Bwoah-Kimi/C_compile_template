//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Created Date:    2025-04-27
// Description:     C file for data storage via CPU.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "store_data.h"
#include "uart.h"
#include "address_map.h"

// void verify_sram_data(volatile uint64_t* sram_addr, const uint64_t* expected_data, uint64_t count, uint64_t flag_value) {
//     asm volatile (
//         "mv t6, %3          \n"  // 使用 t6 寄存器存储初始值
//         "mv t0, %0          \n"  // 预期数据地址 -> t0
//         "mv t1, %1          \n"  // SRAM地址 -> t1
//         "mv t2, %2          \n"  // 计数器 -> t2
//         "1:                 \n"  // 循环开始
//         "   beqz t2, 3f     \n"  // 如果计数器为0，跳转到结束
//         "   ld t3, 0(t0)    \n"  // 加载预期数据
//         "   ld t4, 0(t1)    \n"  // 加载SRAM数据
//         "   bne t3, t4, 2f  \n"  // 如果数据不匹配，跳转到失败
//         "   addi t0, t0, 8  \n"  // 预期数据地址+8
//         "   addi t1, t1, 8  \n"  // SRAM地址+8
//         "   addi t2, t2, -1 \n"  // 计数器-1
//         "   j 1b            \n"  // 继续循环
//         "2:                 \n"  // 验证失败
//         "   li t6, 0        \n"  // 设置验证失败标志
//         // "1:                 \n"  // 死循环开始
//         // "   j 1b            \n"  // 跳转到自己
//         "3:                 \n"  // 结束
//         :                           // 无输出
//     : "r" (expected_data), "r" (sram_addr), "r" (count), "r" (flag_value)  // 输入：预期数据、SRAM地址、计数器、初始值
//         : "t0", "t1", "t2", "t3", "t4", "t6", "memory"      // 使用的寄存器和内存
//         );
// }

void store_sensor_data(uint8_t itr)
{
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    // Read sensor data from therm top
    print_uart("SENSOR ITR ");
    print_uart_byte(itr);
    print_uart(":\n");
    uint64_t* sensor_data_base_addr = (uint64_t*)SENSOR_ENCODINGS_BASE_ADDR; // Base address of sensor data
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
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG; // Set finish flag
    return;
}

void store_pred_data(uint8_t itr)
{
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

    // verify_sram_data((volatile uint64_t*)0x60006000, &dequantized_thermal_data[(int)itr * 14], 14, 0x00000000C2C2C2C2);

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_PRED_FINISH_FLAG; // Set finish flag
}

void store_rl_action(uint8_t itr)
{
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_ACTION_START_FLAG) {}

    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_ACTION_FINISH_FLAG; // Set finish flag
}
