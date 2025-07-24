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


// 读取 IVT Sensor 数据

// UART 通过 AXI 总线访问 SENSOR_ENCODINGS_BASE_ADDR，for 循环顺序访问 14 个 IVT Sensor 的数据
// 每次输出一个 IVT Sensor 的 IVT Code（64-bit）
// sensor_data = {
//      2'b0,
//      power_code, (26-bit)
//      current_code, (16-bit)
//      voltage_code, (10-bit)
//      thermal_code  (10-bit)
//  };

// UART 通过 AXI 总线访问 SENSOR_FREQ_BUFFER_BASE_ADDR，for 循环顺序访问 14 个 IVT Sensor 的数据
// 每次输出一个 IVT Sensor 的 Freq0/Freq1/Freq2（32-bit）
// freq_data = {
//      freq_2, (11-bit)
//      freq_1, (11-bit)
//      freq_0, (10-bit)
// };
void store_sensor_data(uint8_t itr, uint8_t read_code, uint8_t read_freq)
{
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    if (read_code) {
        // Read sensor data from therm top
        print_uart("S");
        print_uart_byte(itr);
        print_uart(":");
        uint64_t* sensor_data_base_addr = (uint64_t*)SENSOR_ENCODINGS_BASE_ADDR;
        for (int j = 0; j < 14; j++)
        {
            uint64_t sensor_data = *(sensor_data_base_addr + j);

            // Extract individual encodings according to the bit layout
            uint16_t thermal_code = (uint16_t)(sensor_data & 0x3FF);           // [9:0]
            uint16_t voltage_code = (uint16_t)((sensor_data >> 10) & 0x3FF);   // [19:10]
            uint16_t current_code = (uint16_t)((sensor_data >> 20) & 0xFFFF);  // [35:20]
            uint32_t power_code = (uint32_t)((sensor_data >> 36) & 0x3FFFFFF); // [61:36]

            // Compact format: T,V,C,P|
            print_uart_int((uint32_t)thermal_code);
            print_uart(",");
            print_uart_int((uint32_t)voltage_code);
            print_uart(",");
            print_uart_int((uint32_t)current_code);
            print_uart(",");
            print_uart_int(power_code);
            print_uart("|");
        }
        print_uart("\n");
    }

    if (read_freq) {
        // Read frequency data from therm top
        print_uart("F");
        print_uart_byte(itr);
        print_uart(":");
        uint32_t* freq_data_base_addr = (uint32_t*)SENSOR_FREQ_BUFFER_BASE_ADDR;
        for (int j = 0; j < 14; j++)
        {
            uint32_t freq_data = *(freq_data_base_addr + j);

            // Extract frequency values according to the bit layout
            uint16_t freq_0 = (uint16_t)(freq_data & 0x3FF);           // [9:0] - 10-bit
            uint16_t freq_1 = (uint16_t)((freq_data >> 10) & 0x7FF);   // [20:10] - 11-bit
            uint16_t freq_2 = (uint16_t)((freq_data >> 21) & 0x7FF);   // [31:21] - 11-bit

            // Compact format: F0,F1,F2|
            print_uart_int((uint32_t)freq_0);
            print_uart(",");
            print_uart_int((uint32_t)freq_1);
            print_uart(",");
            print_uart_int((uint32_t)freq_2);
            print_uart("|");
        }
        print_uart("\n");
    }

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG;
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
