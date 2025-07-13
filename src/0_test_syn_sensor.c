//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     C code for sensor collection function
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"

// Structure to hold all sensor encodings for one iteration
typedef struct {
    uint16_t thermal_encodings[NUM_SENSORS];
    uint16_t current_encodings[NUM_SENSORS];
    uint16_t voltage_encodings[NUM_SENSORS];
} sensor_encodings_t;

void generate_synthetic_encodings(
    uint16_t thermal_data_base,
    uint16_t current_data_base,
    uint16_t voltage_data_base,
    uint32_t itr_count,
    sensor_encodings_t* encodings
) {
    // Generate synthetic encodings for each sensor
    for (int i = 0; i < NUM_SENSORS; i++) {
        encodings->thermal_encodings[i] = thermal_data_base + i + itr_count;
        encodings->voltage_encodings[i] = voltage_data_base + 2 * i + itr_count;
        encodings->current_encodings[i] = current_data_base + 3 * i + itr_count;
    }
}

int main(void) {
    init_uart(10000000, 101000);
    print_uart("Starting sensor collection test...\n");

    uint32_t num_itr = 5;
    // Don't initialize power switch on FPGA
    // init_power_switch();

    // Using UART to transfer sensor data.
    uint16_t synthetic_sensor_thermal_encodings = 0x10;
    uint16_t synthetic_sensor_current_encodings = 0x3;
    uint16_t synthetic_sensor_voltage_encodings = 0x2;

    init_therm_top(
        1,        // therm_top_start
        1,        // therm_top_en
        0,        // therm_top_stop
        1,        // collect_en
        0,        // collect_mode
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
        synthetic_sensor_thermal_encodings,      // synthetic_sensor_thermal_encodings
        synthetic_sensor_current_encodings,      // synthetic_sensor_current_encodings
        synthetic_sensor_voltage_encodings,      // synthetic_sensor_voltage_encodings
        342391,   // synthetic_action_sequence
        0,        // store_pred_mode
        0         // store_action_mode
    );

    sensor_encodings_t expected_encodings;
    sensor_encodings_t read_encodings;
    uint64_t* sensor_data_base_addr = (uint64_t*)SENSOR_ENCODINGS_BASE_ADDR;
    for (uint32_t itr = 0; itr < num_itr; itr++) {
        generate_synthetic_encodings(
            synthetic_sensor_thermal_encodings,
            synthetic_sensor_current_encodings,
            synthetic_sensor_voltage_encodings,
            itr + 1,
            &expected_encodings
        );

        while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

        // Read sensor data from current iteration
        for (int j = 0; j < NUM_SENSORS; j++) {
            uint64_t sensor_data = *(sensor_data_base_addr + j);
            uint16_t thermal_encoding = (uint16_t)(sensor_data & 0x3FF);          // 10-bit
            uint16_t voltage_encoding = (uint16_t)((sensor_data >> 10) & 0x3FF);  // 10-bit
            uint16_t current_encoding = (uint16_t)((sensor_data >> 20) & 0xFFFF);  // 16-bit
            read_encodings.thermal_encodings[j] = thermal_encoding;
            read_encodings.voltage_encodings[j] = voltage_encoding;
            read_encodings.current_encodings[j] = current_encoding;
        }

        // Set finish flag
        *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG; // Set finish flag

        // Print comparison between expected and read values
        print_uart("Iteration ");
        print_uart_byte(itr);
        print_uart(":\n");
        uint8_t match_success = 1;
        for (int j = 0; j < NUM_SENSORS; j++) {
            if (expected_encodings.thermal_encodings[j] == read_encodings.thermal_encodings[j] &&
                expected_encodings.voltage_encodings[j] == read_encodings.voltage_encodings[j] &&
                expected_encodings.current_encodings[j] == read_encodings.current_encodings[j]) {
                // Expected and read values are the same
                continue;
            }
            else {
                // Mismatch found
                match_success = 0;
                print_uart("  Mismatch for Sensor ");
                print_uart_int((uint32_t)j);
                print_uart(":\n");
                print_uart("    Expected Thermal: ");
                print_uart_int((uint32_t)expected_encodings.thermal_encodings[j]);
                print_uart("\n");
                print_uart("    Read Thermal: ");
                print_uart_int((uint32_t)read_encodings.thermal_encodings[j]);
                print_uart("\n");
                print_uart("    Expected Voltage: ");
                print_uart_int((uint32_t)expected_encodings.voltage_encodings[j]);
                print_uart("\n");
                print_uart("    Read Voltage: ");
                print_uart_int((uint32_t)read_encodings.voltage_encodings[j]);
                print_uart("\n");
                print_uart("    Expected Current: ");
                print_uart_int((uint32_t)expected_encodings.current_encodings[j]);
                print_uart("\n");
                print_uart("    Read Current: ");
                print_uart_int((uint32_t)read_encodings.current_encodings[j]);
                print_uart("\n");
            }
        }
        if (match_success) {
            print_uart("  All encodings match for this iteration!\n");
        }
        else {
            print_uart("  Mismatch found in this iteration!\n");
        }
    }

    return 0;
}