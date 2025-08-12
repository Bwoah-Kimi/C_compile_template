//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     C code for test chip basic functionality
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"

//////////////////////////////////////////////////
// Structure to hold all sensor encodings for one iteration
//////////////////////////////////////////////////
typedef struct {
    uint16_t thermal_encodings[NUM_SENSORS];
    uint16_t current_encodings[NUM_SENSORS];
    uint16_t voltage_encodings[NUM_SENSORS];
} sensor_encodings_t;

//////////////////////////////////////////////////
// RL-config regfile test values
// 3 regfiles, each 64-bit
//////////////////////////////////////////////////
static const uint64_t rl_config_test_values[3] = {
    0x1234567890ABCDEF,  // RL config regfile 0
    0xFEDCBA0987654321,  // RL config regfile 1
    0xA5A5A5A55A5A5A5A   // RL config regfile 2
};

//////////////////////////////////////////////////
// Therm_top config regfile test values
// 4 regfiles, each 64-bit
//////////////////////////////////////////////////
static const uint64_t therm_top_config_test_values[4] = {
    0x11223344556677AA,  // Therm_top config regfile 0
    0xBBCCDDEEFF001122,  // Therm_top config regfile 1
    0x3344556677889900,  // Therm_top config regfile 2
    0xAABBCCDDEEFF1234   // Therm_top config regfile 3
};

//////////////////////////////////////////////////
// Quant config regfile test values
// 16 regfiles, each 64-bit
//////////////////////////////////////////////////
static const uint64_t quant_config_test_values[16] = {
    0x1111111111111111,  // Quant config regfile 0
    0x2222222222222222,  // Quant config regfile 1
    0x3333333333333333,  // Quant config regfile 2
    0x4444444444444444,  // Quant config regfile 3
    0x5555555555555555,  // Quant config regfile 4
    0x6666666666666666,  // Quant config regfile 5
    0x7777777777777777,  // Quant config regfile 6
    0x8888888888888888,  // Quant config regfile 7
    0x9999999999999999,  // Quant config regfile 8
    0xAAAAAAAAAAAAAAAA,  // Quant config regfile 9
    0xBBBBBBBBBBBBBBBB,  // Quant config regfile 10
    0xCCCCCCCCCCCCCCCC,  // Quant config regfile 11
    0xDDDDDDDDDDDDDDDD,  // Quant config regfile 12
    0xEEEEEEEEEEEEEEEE,  // Quant config regfile 13
    0xFFFFFFFFFFFFFFFF,  // Quant config regfile 14
    0x0F0F0F0F0F0F0F0F   // Quant config regfile 15
};

//////////////////////////////////////////////////
// Dequant config regfile test values
// 16 regfiles, each 64-bit
//////////////////////////////////////////////////
static const uint64_t dequant_config_test_values[16] = {
    0xF0F0F0F0F0F0F0F0,  // Dequant config regfile 0
    0xE1E1E1E1E1E1E1E1,  // Dequant config regfile 1
    0xD2D2D2D2D2D2D2D2,  // Dequant config regfile 2
    0xC3C3C3C3C3C3C3C3,  // Dequant config regfile 3
    0xB4B4B4B4B4B4B4B4,  // Dequant config regfile 4
    0xA5A5A5A5A5A5A5A5,  // Dequant config regfile 5
    0x9696969696969696,  // Dequant config regfile 6
    0x8787878787878787,  // Dequant config regfile 7
    0x7878787878787878,  // Dequant config regfile 8
    0x6969696969696969,  // Dequant config regfile 9
    0x5A5A5A5A5A5A5A5A,  // Dequant config regfile 10
    0x4B4B4B4B4B4B4B4B,  // Dequant config regfile 11
    0x3C3C3C3C3C3C3C3C,  // Dequant config regfile 12
    0x2D2D2D2D2D2D2D2D,  // Dequant config regfile 13
    0x1E1E1E1E1E1E1E1E,  // Dequant config regfile 14
    0x0123456789ABCDEF   // Dequant config regfile 15
};


void generate_synthetic_encodings(uint16_t thermal_data_base, uint16_t current_data_base, uint16_t voltage_data_base, uint32_t itr_count, sensor_encodings_t* encodings) {
    // Generate synthetic encodings for each sensor
    for (int i = 0; i < NUM_SENSORS; i++) {
        encodings->thermal_encodings[i] = thermal_data_base + i + itr_count;
        encodings->voltage_encodings[i] = voltage_data_base + 2 * i + itr_count;
        encodings->current_encodings[i] = current_data_base + 3 * i + itr_count;
    }
}


void init_test_freq_buffer(void) {
    // Initialize sensor frequency buffer with test pattern
    // 16 entries, lower 32-bit containing 3 frequency values
    uint64_t* freq_buffer_addr = (uint64_t*)SENSOR_FREQ_BUFFER_BASE_ADDR;

    for (int i = 0; i < 16; i++) {
        // Create test pattern: freq_0 = i*10, freq_1 = i*10+1, freq_2 = i*10+2
        uint16_t freq_0 = (uint16_t)(i * 10);      // [9:0] - 10-bit
        uint16_t freq_1 = (uint16_t)(i * 10 + 1);  // [20:10] - 11-bit  
        uint16_t freq_2 = (uint16_t)(i * 10 + 2);  // [31:21] - 11-bit

        // Pack frequencies into 64-bit word
        uint64_t freq_data = ((uint64_t)(freq_2 & 0x7FF) << 21) |
            ((uint64_t)(freq_1 & 0x7FF) << 10) |
            ((uint64_t)(freq_0 & 0x3FF));

        *(freq_buffer_addr + i) = freq_data;
    }
}


void init_test_q_tables(void) {
    // Initialize Q-table 0 with test pattern
    // Each 64-bit word contains 4 actions (4 × 16-bit = 64-bit)
    uint64_t* q_table_0_addr = (uint64_t*)Q_TABLE_0_BASE_ADDR;
    for (int state = 0; state < NUM_STATES; state++) {
        uint64_t q_values = 0;
        // Pack 4 actions into one 64-bit word
        for (int action = 0; action < ACTION_PER_TABLE; action++) {
            int16_t test_value = (int16_t)(state * 10 + action);
            q_values |= ((uint64_t)(test_value & 0xFFFF) << (action * 16));
        }
        *(q_table_0_addr + state) = q_values;
    }

    // Initialize Q-table 1 with different test pattern
    uint64_t* q_table_1_addr = (uint64_t*)Q_TABLE_1_BASE_ADDR;
    for (int state = 0; state < NUM_STATES; state++) {
        uint64_t q_values = 0;
        // Pack 4 actions into one 64-bit word
        for (int action = 0; action < ACTION_PER_TABLE; action++) {
            int16_t test_value = (int16_t)(1000 + state * 10 + action);
            q_values |= ((uint64_t)(test_value & 0xFFFF) << (action * 16));
        }
        *(q_table_1_addr + state) = q_values;
    }
}


void init_test_config_regfiles(void) {
    // Initialize RL config regfiles
    uint64_t* rl_config_addr = (uint64_t*)RL_CONFIG_REGFILE_BASE_ADDR;
    for (int i = 0; i < 3; i++)
    {
        *(rl_config_addr + i) = rl_config_test_values[i];
    }

    // Initialize therm_top config regfiles
    uint64_t* therm_top_config_addr = (uint64_t*)TOP_CONFIG_REGFILE_BASE_ADDR;
    for (int i = 0; i < 4; i++)
    {
        *(therm_top_config_addr + i) = therm_top_config_test_values[i];
    }

    // Initialize quant config regfiles
    uint64_t* quant_config_addr = (uint64_t*)QUANT_CONFIG_REGFILE_BASE_ADDR;
    for (int i = 0; i < 16; i++)
    {
        *(quant_config_addr + i) = quant_config_test_values[i];
    }

    // Initialize dequant config regfiles
    uint64_t* dequant_config_addr = (uint64_t*)DEQUANT_CONFIG_REGFILE_BASE_ADDR;
    for (int i = 0; i < 16; i++)
    {
        *(dequant_config_addr + i) = dequant_config_test_values[i];
    }
}


void test_mem_layout(void) {
    print_uart("\n================================================\n");
    print_uart("Starting Memory Layout Verification Test\n");
    print_uart("================================================\n");

    uint32_t total_errors = 0;

    //////////////////////////////////////////////////
    // Test power switch values
    //////////////////////////////////////////////////
    // print_uart("Testing Power Switch Memory Layout\n");
    // uint64_t* power_switch_addr = (uint64_t*)POWER_SWITCH_BASE_ADDR;
    // uint32_t power_switch_errors = 0;
    // for (int i = 0; i < NUM_POWER_SWITCHES; i++)
    // {
    //     uint8_t read_value = *(power_switch_addr + i);
    //     uint8_t expected_value = power_switch_config[i];
    //     if (read_value != expected_value)
    //     {
    //         power_switch_errors++;
    //         total_errors++;
    //         print_uart("  Power Switch[");
    //         print_uart_int((uint32_t)i);
    //         print_uart("] MISMATCH:\n");
    //         print_uart("    Expected: ");
    //         print_uart_int((uint32_t)expected_value);
    //         print_uart("\n");
    //         print_uart("    Read: ");
    //         print_uart_int((uint32_t)read_value);
    //         print_uart("\n");
    //     }
    // }

    // if (power_switch_errors == 0)
    // {
    //     print_uart("  All power switches match expected values.\n");
    // }
    // else
    // {
    //     print_uart("  Power switch test completed with ");
    //     print_uart_int(power_switch_errors);
    //     print_uart(" errors.\n");
    // }

    //////////////////////////////////////////////////
    // Test sensor weight buffer memory layout
    //////////////////////////////////////////////////
    print_uart("Testing Sensor Weight Buffer Memory Layout\n");
    uint64_t* sensor_weight_addr = (uint64_t*)SENSOR_WEIGHT_BUFFER_BASE_ADDR;
    uint32_t sensor_weight_errors = 0;
    for (int i = 0; i < 22; i++)
    {
        int64_t read_value = *(sensor_weight_addr + i);
        int64_t expected_value = sensor_weight[i];
        if (read_value != expected_value)
        {
            sensor_weight_errors++;
            total_errors++;
            print_uart("  Sensor Weight[");
            print_uart_int((uint32_t)i);
            print_uart("] MISMATCH:\n");
            print_uart("    Expected: ");
            print_uart_addr((int64_t)expected_value);
            print_uart("\n");
            print_uart("    Read: ");
            print_uart_addr((int64_t)read_value);
            print_uart("\n");
        }
    }

    if (sensor_weight_errors == 0)
    {
        print_uart("  All sensor weights match expected values.\n");
    }
    else
    {
        print_uart("  Sensor weight test completed with ");
        print_uart_int(sensor_weight_errors);
        print_uart(" errors.\n");
    }

    //////////////////////////////////////////////////
    // Test sensor frequency buffer memory layout
    //////////////////////////////////////////////////
    print_uart("Testing Sensor Frequency Buffer Memory Layout\n");

    // Initialize frequency buffer with test values
    init_test_freq_buffer();

    uint64_t* freq_buffer_addr = (uint64_t*)SENSOR_FREQ_BUFFER_BASE_ADDR;
    uint32_t freq_buffer_errors = 0;

    // Test all 16 frequency buffer entries
    for (int i = 0; i < 16; i++) {
        uint64_t read_freq_data = *(freq_buffer_addr + i);

        // Extract individual frequency values
        uint16_t read_freq_0 = (uint16_t)(read_freq_data & 0x3FF);           // [9:0] - 10-bit
        uint16_t read_freq_1 = (uint16_t)((read_freq_data >> 10) & 0x7FF);   // [20:10] - 11-bit
        uint16_t read_freq_2 = (uint16_t)((read_freq_data >> 21) & 0x7FF);   // [31:21] - 11-bit

        // Calculate expected values
        uint16_t expected_freq_0 = (uint16_t)(i * 10);
        uint16_t expected_freq_1 = (uint16_t)(i * 10 + 1);
        uint16_t expected_freq_2 = (uint16_t)(i * 10 + 2);

        // Check if any frequency value mismatches
        if (read_freq_0 != expected_freq_0 ||
            read_freq_1 != expected_freq_1 ||
            read_freq_2 != expected_freq_2) {

            freq_buffer_errors++;
            total_errors++;
            if (freq_buffer_errors <= 5) { // Only show first 5 errors to avoid spam
                print_uart("  Freq Buffer[");
                print_uart_int((uint32_t)i);
                print_uart("] MISMATCH:\n");
                print_uart("    Expected F0,F1,F2: ");
                print_uart_int((uint32_t)expected_freq_0);
                print_uart(",");
                print_uart_int((uint32_t)expected_freq_1);
                print_uart(",");
                print_uart_int((uint32_t)expected_freq_2);
                print_uart("\n");
                print_uart("    Read F0,F1,F2: ");
                print_uart_int((uint32_t)read_freq_0);
                print_uart(",");
                print_uart_int((uint32_t)read_freq_1);
                print_uart(",");
                print_uart_int((uint32_t)read_freq_2);
                print_uart("\n");
                print_uart("    Raw Data: ");
                print_uart_addr((uint64_t)read_freq_data);
                print_uart("\n");
            }
        }
    }

    // Report results
    if (freq_buffer_errors == 0) {
        print_uart("  All 16 sensor frequency buffer entries match expected values.\n");
    }
    else {
        print_uart("  Sensor frequency buffer test completed with ");
        print_uart_int(freq_buffer_errors);
        print_uart(" errors.\n");
    }

    //////////////////////////////////////////////////
    // Test Q-Table values
    //////////////////////////////////////////////////
    print_uart("Testing Q-Table Memory Layout\n");

    // Initialize Q-tables with test values
    init_test_q_tables();

    uint64_t* q_table_0_addr = (uint64_t*)Q_TABLE_0_BASE_ADDR;
    uint64_t* q_table_1_addr = (uint64_t*)Q_TABLE_1_BASE_ADDR;

    uint32_t q_table_0_errors = 0;
    uint32_t q_table_1_errors = 0;

    // Test Q-table 0 (512 states, each containing 4 actions)
    for (int state = 0; state < NUM_STATES; state++) {
        uint64_t read_q_values = *(q_table_0_addr + state);

        // Extract and verify each action
        for (int action = 0; action < ACTION_PER_TABLE; action++) {
            int16_t expected_value = (int16_t)(state * 10 + action);
            int16_t read_value = (int16_t)((read_q_values >> (action * 16)) & 0xFFFF);

            if (read_value != expected_value) {
                q_table_0_errors++;
                total_errors++;
                if (q_table_0_errors <= 5) { // Only show first 5 errors to avoid spam
                    print_uart("  Q-Table 0[");
                    print_uart_int((uint32_t)state);
                    print_uart("][");
                    print_uart_int((uint32_t)action);
                    print_uart("] MISMATCH:\n");
                    print_uart("    Expected: ");
                    print_uart_int((uint32_t)expected_value);
                    print_uart("\n");
                    print_uart("    Read: ");
                    print_uart_int((uint32_t)read_value);
                    print_uart("\n");
                }
            }
        }
    }

    // Test Q-table 1 (512 states, each containing 4 actions)
    for (int state = 0; state < NUM_STATES; state++) {
        uint64_t read_q_values = *(q_table_1_addr + state);

        // Extract and verify each action
        for (int action = 0; action < ACTION_PER_TABLE; action++) {
            int16_t expected_value = (int16_t)(1000 + state * 10 + action);
            int16_t read_value = (int16_t)((read_q_values >> (action * 16)) & 0xFFFF);

            if (read_value != expected_value) {
                q_table_1_errors++;
                total_errors++;
                if (q_table_1_errors <= 5) { // Only show first 5 errors to avoid spam
                    print_uart("  Q-Table 1[");
                    print_uart_int((uint32_t)state);
                    print_uart("][");
                    print_uart_int((uint32_t)action);
                    print_uart("] MISMATCH:\n");
                    print_uart("    Expected: ");
                    print_uart_int((uint32_t)expected_value);
                    print_uart("\n");
                    print_uart("    Read: ");
                    print_uart_int((uint32_t)read_value);
                    print_uart("\n");
                }
            }
        }
    }

    // Report results
    if (q_table_0_errors == 0) {
        print_uart("  Q-Table 0: All 512 states (2048 Q-values) match expected pattern.\n");
    }
    else {
        print_uart("  Q-Table 0: ");
        print_uart_int(q_table_0_errors);
        print_uart(" errors found.\n");
    }

    if (q_table_1_errors == 0) {
        print_uart("  Q-Table 1: All 512 states (2048 Q-values) match expected pattern.\n");
    }
    else {
        print_uart("  Q-Table 1: ");
        print_uart_int(q_table_1_errors);
        print_uart(" errors found.\n");
    }

    //////////////////////////////////////////////////
    // Test config regfiles
    //////////////////////////////////////////////////
    print_uart("Testing Config Regfiles Memory Layout\n");

    // Initialize config regfiles with test values
    init_test_config_regfiles();

    uint64_t* rl_config_addr = (uint64_t*)RL_CONFIG_REGFILE_BASE_ADDR;
    uint64_t* therm_top_config_addr = (uint64_t*)TOP_CONFIG_REGFILE_BASE_ADDR;
    uint64_t* quant_config_addr = (uint64_t*)QUANT_CONFIG_REGFILE_BASE_ADDR;
    uint64_t* dequant_config_addr = (uint64_t*)DEQUANT_CONFIG_REGFILE_BASE_ADDR;

    uint32_t rl_config_errors = 0;
    uint32_t therm_top_config_errors = 0;
    uint32_t quant_config_errors = 0;
    uint32_t dequant_config_errors = 0;

    // Test RL config regfiles (3 regfiles)
    for (int i = 0; i < 3; i++) {
        uint64_t read_value = *(rl_config_addr + i);
        uint64_t expected_value = rl_config_test_values[i];

        if (read_value != expected_value) {
            rl_config_errors++;
            total_errors++;
            print_uart("  RL Config[");
            print_uart_int((uint32_t)i);
            print_uart("] MISMATCH:\n");
            print_uart("    Expected: ");
            print_uart_addr(expected_value);
            print_uart("\n");
            print_uart("    Read: ");
            print_uart_addr(read_value);
            print_uart("\n");
        }
    }

    // Test therm_top config regfiles (4 regfiles)
    for (int i = 0; i < 4; i++) {
        uint64_t read_value = *(therm_top_config_addr + i);
        uint64_t expected_value = therm_top_config_test_values[i];

        if (read_value != expected_value) {
            therm_top_config_errors++;
            total_errors++;
            print_uart("  Therm_top Config[");
            print_uart_int((uint32_t)i);
            print_uart("] MISMATCH:\n");
            print_uart("    Expected: ");
            print_uart_addr(expected_value);
            print_uart("\n");
            print_uart("    Read: ");
            print_uart_addr(read_value);
            print_uart("\n");
        }
    }

    // Test quant config regfiles (16 regfiles)
    for (int i = 0; i < 16; i++) {
        uint64_t read_value = *(quant_config_addr + i);
        uint64_t expected_value = quant_config_test_values[i];

        if (read_value != expected_value) {
            quant_config_errors++;
            total_errors++;
            if (quant_config_errors <= 5) { // Only show first 5 errors to avoid spam
                print_uart("  Quant Config[");
                print_uart_int((uint32_t)i);
                print_uart("] MISMATCH:\n");
                print_uart("    Expected: ");
                print_uart_addr(expected_value);
                print_uart("\n");
                print_uart("    Read: ");
                print_uart_addr(read_value);
                print_uart("\n");
            }
        }
    }

    // Test dequant config regfiles (16 regfiles)
    for (int i = 0; i < 16; i++) {
        uint64_t read_value = *(dequant_config_addr + i);
        uint64_t expected_value = dequant_config_test_values[i];

        if (read_value != expected_value) {
            dequant_config_errors++;
            total_errors++;
            if (dequant_config_errors <= 5) { // Only show first 5 errors to avoid spam
                print_uart("  Dequant Config[");
                print_uart_int((uint32_t)i);
                print_uart("] MISMATCH:\n");
                print_uart("    Expected: ");
                print_uart_addr(expected_value);
                print_uart("\n");
                print_uart("    Read: ");
                print_uart_addr(read_value);
                print_uart("\n");
            }
        }
    }

    // Report results
    if (rl_config_errors == 0) {
        print_uart("  All 3 RL config regfiles match expected values.\n");
    }
    else {
        print_uart("  RL config test completed with ");
        print_uart_int(rl_config_errors);
        print_uart(" errors.\n");
    }

    if (therm_top_config_errors == 0) {
        print_uart("  All 4 therm_top config regfiles match expected values.\n");
    }
    else {
        print_uart("  Therm_top config test completed with ");
        print_uart_int(therm_top_config_errors);
        print_uart(" errors.\n");
    }

    if (quant_config_errors == 0) {
        print_uart("  All 16 quant config regfiles match expected values.\n");
    }
    else {
        print_uart("  Quant config test completed with ");
        print_uart_int(quant_config_errors);
        print_uart(" errors.\n");
    }

    if (dequant_config_errors == 0) {
        print_uart("  All 16 dequant config regfiles match expected values.\n");
    }
    else {
        print_uart("  Dequant config test completed with ");
        print_uart_int(dequant_config_errors);
        print_uart(" errors.\n");
    }

    //////////////////////////////////////////////////
    // Final report
    //////////////////////////////////////////////////
    if (total_errors == 0) {
        print_uart("=================================================\n");
        print_uart("Memory Layout Test PASSED.");
        print_uart("\n=================================================\n");
    }
    else {
        print_uart("=================================================\n");
        print_uart("Memory Layout Test FAILED with ");
        print_uart_int(total_errors);
        print_uart(" errors.\n");
        print_uart("=================================================\n");
    }

    return;
}


void test_synthetic_sensor_data_collection(void) {
    print_uart("\n================================================\n");
    print_uart("Starting Synthetic Sensor Data Collection Test\n");
    print_uart("================================================\n");

    uint32_t num_itr = 5;

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
    uint32_t error_count = 0;
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
        for (int j = 0; j < NUM_SENSORS; j++) {
            if (expected_encodings.thermal_encodings[j] == read_encodings.thermal_encodings[j] &&
                expected_encodings.voltage_encodings[j] == read_encodings.voltage_encodings[j] &&
                expected_encodings.current_encodings[j] == read_encodings.current_encodings[j]) {
                // Expected and read values are the same
                continue;
            }
            else {
                // Mismatch found
                print_uart("Iteration ");
                print_uart_byte(itr);
                print_uart(":\n");
                error_count++;
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
    }

    if (error_count == 0) {
        print_uart("=================================================\n");
        print_uart("Synthetic Sensor Data Collection Test PASSED.\n");
        print_uart("=================================================\n");
    }
    else {
        print_uart("=================================================\n");
        print_uart("Synthetic Sensor Data Collection Test FAILED with ");
        print_uart_int(error_count);
        print_uart(" errors.\n");
        print_uart("================================================\n");
    }
    return;
}

void test_synthetic_task_scheduling(void) {
    print_uart("\n================================================\n");
    print_uart("Starting Synthetic Task Scheduling Test\n");
    print_uart("================================================\n");

    uint32_t num_itr = 5;

    init_therm_top(
        1,        // therm_top_start
        1,        // therm_top_en
        0,        // therm_top_stop
        0,        // collect_en
        0,        // collect_mode
        0,        // pred_en
        0,        // pred_mode
        1,        // schedule_en
        1,        // store_sensor_mode
        4,        // action_offset
        num_itr,  // num_itr
        10000,    // sampling_intvl
        0xA000,   // sensor_data_base_addr
        0xB000,   // pred_data_base_addr
        0xC000,   // action_base_addr
        0x0,      // npu_input_buf_base_addr
        0x8,      // npu_output_buf_base_addr
        0x0,      // synthetic_sensor_thermal_encodings
        0x0,      // synthetic_sensor_current_encodings
        0x0,      // synthetic_sensor_voltage_encodings
        342391,   // synthetic_action_sequence
        0,        // store_pred_mode
        0         // store_action_mode
    );

    print_uart("=================================================\n");
    print_uart("Synthetic Task Scheduling Test FINISHED.\n");
    print_uart("=================================================\n");
    return;
}


int main(void) {
    init_uart(115000000, 115200);

    // Test memory layout
    test_mem_layout();

    // Test synthetic sensor data collection
    test_synthetic_sensor_data_collection();
}
