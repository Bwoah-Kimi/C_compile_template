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

void conv_and_store_sensor_code_to_mem(uint32_t itr, uint32_t sensor_data_base_addr) {
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    process_sensor_data(itr, sensor_data_base_addr);

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG;
    return;
}

void store_sensor_freq_to_mem(int itr, uint32_t sensor_data_base_addr) {
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    uint64_t* freq_data_base_addr = (uint64_t*)SENSOR_FREQ_BUFFER_BASE_ADDR;
    uint64_t* store_addr = (uint64_t*)(uint64_t)(0x80000000 + sensor_data_base_addr);
    for (int j = 0; j < 14; j++)
    {
        uint64_t freq_data = *(freq_data_base_addr + j);

        // Extract frequency values according to the bit layout
        uint16_t freq_0 = (uint16_t)(freq_data & 0x3FF);           // [9:0] - 10-bit
        uint16_t freq_1 = (uint16_t)((freq_data >> 10) & 0x7FF);   // [20:10] - 11-bit
        uint16_t freq_2 = (uint16_t)((freq_data >> 21) & 0x7FF);   // [31:21] - 11-bit

        *(uint64_t*)(store_addr + 3 * 14 * itr + 3 * j + 0) = freq_0;
        *(uint64_t*)(store_addr + 3 * 14 * itr + 3 * j + 1) = freq_1;
        *(uint64_t*)(store_addr + 3 * 14 * itr + 3 * j + 2) = freq_2;
    }

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG;
    return;
}

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

void _initialize_sensor_weights(SensorWeights weights[14]) {
    for (int k = 0; k < 14; k++) {
        for (int i = 0; i < 6; i++) {
            weights[k].weightV[i] = per_sensor_weights[k].weightV[i];
            weights[k].weightT[i] = per_sensor_weights[k].weightT[i];
        }
        for (int i = 0; i < 10; i++) {
            weights[k].weightI[i] = per_sensor_weights[k].weightI[i];
        }
    }
    return;
}

void process_sensor_data(uint32_t itr_count, uint32_t sensor_data_base_addr) {
    // Read frequency data for all 14 sensors
    uint16_t freq_0[14], freq_1[14], freq_2[14];
    for (int j = 0; j < 14; j++) {
        uint64_t freq_data = *(volatile uint64_t*)(SENSOR_FREQ_BUFFER_BASE_ADDR + j * sizeof(uint64_t));
        freq_0[j] = (uint16_t)(freq_data & 0x3FF);         // [9:0] - 10-bit
        freq_1[j] = (uint16_t)((freq_data >> 10) & 0x7FF); // [20:10] - 11-bit
        freq_2[j] = (uint16_t)((freq_data >> 21) & 0x7FF); // [31:21] - 11-bit
    }

    // Read power switch codes
    // TODO
    uint32_t ps_code[14];
    for (int k = 0; k < 14; k++) {
        ps_code[k] = *(volatile uint32_t*)(POWER_SWITCH_BASE_ADDR + k * sizeof(uint32_t));
    }

    // Initialize weights (can choose memory or defaults)
    SensorWeights weights[14];
    _initialize_sensor_weights(weights);

    // Calculate V, T, I for each sensor using 128-bit arithmetic
    __int128 V[14], T[14], I[14];

    for (int k = 0; k < 14; k++) {
        uint16_t f0 = freq_0[k];
        uint16_t f1 = freq_1[k];
        uint16_t f2 = freq_2[k];
        uint16_t f3 = f2 - f1;  // freq_3 = freq_2 - freq_1 (guaranteed > 0)

        // Calculate V[k] (6 terms)
        __int128 v = (__int128)weights[k].weightV[0] * 1024;
        v += (__int128)weights[k].weightV[1] * f0;
        v += (__int128)weights[k].weightV[2] * f1;
        v += (__int128)weights[k].weightV[3] * f0 * f0;
        v += (__int128)weights[k].weightV[4] * f0 * f1;
        v += (__int128)weights[k].weightV[5] * f1 * f1;
        V[k] = v;  // Result is guaranteed positive

        // Calculate T[k] (6 terms)
        __int128 t = (__int128)weights[k].weightT[0] * 1024;
        t += (__int128)weights[k].weightT[1] * f0;
        t += (__int128)weights[k].weightT[2] * f1;
        t += (__int128)weights[k].weightT[3] * f0 * f0;
        t += (__int128)weights[k].weightT[4] * f0 * f1;
        t += (__int128)weights[k].weightT[5] * f1 * f1;
        T[k] = t;  // Result is guaranteed positive

        // Calculate I[k] (10 terms + ps_code * freq_3)
        __int128 i_part = (__int128)weights[k].weightI[0] * 1024;
        i_part += (__int128)weights[k].weightI[1] * f0;
        i_part += (__int128)weights[k].weightI[2] * f1;
        i_part += (__int128)weights[k].weightI[3] * f2;
        i_part += (__int128)weights[k].weightI[4] * f0 * f0;
        i_part += (__int128)weights[k].weightI[5] * f1 * f1;
        i_part += (__int128)weights[k].weightI[6] * f2 * f2;
        i_part += (__int128)weights[k].weightI[7] * f0 * f1;
        i_part += (__int128)weights[k].weightI[8] * f1 * f2;
        i_part += (__int128)weights[k].weightI[9] * f2 * f0;

        // Final I calculation with ps_code and freq_3
        I[k] = (__int128)ps_code[k] * (__int128)f3 * i_part;
    }

    // Extract and pack the required bit fields
    for (int k = 0; k < 14; k++) {
        // Extract the required bit ranges
        uint16_t sensor_current_encodings = (I[k] >> 26) & 0xFFFF;  // I[k][41:26] -> 16 bits
        uint16_t sensor_voltage_encodings = (V[k] >> 24) & 0x3FF;    // V[k][33:24] -> 10 bits
        uint16_t sensor_thermal_encodings = (T[k] >> 24) & 0x3FF;    // T[k][33:24] -> 10 bits

        // Calculate power (example: P = V * I, scaled appropriately)
        uint32_t sensor_power_encodings = (uint64_t)sensor_voltage_encodings * sensor_current_encodings; // Scale down and take 26 bits
        sensor_power_encodings &= 0x3FFFFFF; // Ensure 26 bits

        // Pack into 64-bit word
        uint64_t sensor_data_wdata =
            ((uint64_t)(sensor_power_encodings & 0x3FFFFFF) << 36) |  // Power [61:36] (26 bits)
            ((uint64_t)(sensor_current_encodings & 0xFFFF) << 20) |   // Current [35:20] (16 bits)
            ((uint64_t)(sensor_voltage_encodings & 0x3FF) << 10) |    // Voltage [19:10] (10 bits)
            ((uint64_t)(sensor_thermal_encodings & 0x3FF));           // Thermal [9:0] (10 bits)

        // Calculate main memory destination address
        uint64_t sensor_data_addr = (uint64_t)(sensor_data_base_addr + 0x80000000ULL) +
            (uint64_t)itr_count * 14 * sizeof(uint64_t) +
            (uint64_t)k * sizeof(uint64_t);

        // Store to memory
        *(volatile uint64_t*)sensor_data_addr = sensor_data_wdata;
    }

    return;
}