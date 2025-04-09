#include <stdint.h>
#include "../include/create_regfile_data.h"

void init_therm_top(void)
{
    // Top config regfile 0
    uint8_t therm_top_start = 0;
    uint8_t therm_top_en = 1;
    uint8_t therm_top_stop = 0;
    uint8_t collect_en = 1;
    uint8_t collect_mode = 0;
    uint8_t pred_en = 0;
    uint8_t schedule_en = 0;
    uint8_t store_sensor_en = 1;
    uint8_t store_pred_en = 0;
    uint8_t store_action_en = 0;
    uint8_t action_offset = 4;
    uint32_t num_itr = 100;
    uint32_t sampling_intvl = 100;

    // Top config regfile 1
    uint16_t action_base_addr = 0x0000;
    uint16_t sensor_data_base_addr = 0x0000;
    uint16_t pred_data_base_addr = 0x0000;

    // Top config regfile 2
    uint16_t npu_input_buf_base_addr = 0x0000;
    uint16_t npu_output_buf_base_addr = 0x0000;

    uint8_t synthetic_sensor_thermal_encodings = 0x00;
    uint8_t synthetic_sensor_current_encodings = 0x00;
    uint8_t synthetic_sensor_voltage_encodings = 0x00;

    // Top config regfile 3
    uint32_t synthetic_action_sequence = 0x00000000;

    uint64_t top_regfile_0_data = create_top_regfile_0_data(
        therm_top_start, therm_top_en, therm_top_stop, collect_en,
        collect_mode, pred_en, schedule_en, store_sensor_en,
        store_pred_en, store_action_en, action_offset, num_itr,
        sampling_intvl);

    uint64_t top_regfile_1_data = create_top_regfile_1_data(
        action_base_addr, sensor_data_base_addr, pred_data_base_addr);

    uint64_t top_regfile_2_data = create_top_regfile_2_data(
        npu_input_buf_base_addr, npu_output_buf_base_addr,
        synthetic_sensor_thermal_encodings,
        synthetic_sensor_current_encodings,
        synthetic_sensor_voltage_encodings);

    uint64_t top_regfile_3_data = create_top_regfile_3_data(synthetic_action_sequence);

    // Write the data to respective registers

    uint64_t *top_regfile_base_addr = (uint64_t *)0x60002218; // Base address of top register file
    *(top_regfile_base_addr + 0) = top_regfile_0_data;        // Write to regfile 0
    *(top_regfile_base_addr + 1) = top_regfile_1_data;        // Write to regfile 1
    *(top_regfile_base_addr + 2) = top_regfile_2_data;        // Write to regfile 2
    *(top_regfile_base_addr + 3) = top_regfile_3_data;        // Write to regfile 3
}

void init_rl_scheduler(void)
{
    // RL config regfile 0
    uint16_t coef_k1 = 0x0123;
    uint16_t coef_k2 = 0x4567;
    uint16_t coef_k3 = 0x89AB;
    uint16_t learning_rate = 0x0080; // 0.5 in fixed-point representation

    // RL config regfile 1
    uint16_t discount_factor = 0x00CC; // ~0.8 in fixed-point representation
    uint16_t initial_epsilon = 0x00FF; // ~1.0 in fixed-point representation
    uint8_t num_active_actions = 4;
    uint8_t thres_high = 80; // Temperature threshold in Celsius
    uint8_t thres_mid = 60;  // Temperature threshold in Celsius
    uint8_t thres_low = 40;  // Temperature threshold in Celsius

    // RL config regfile 2
    uint16_t epsilon_decay_step = 0x000A;   // Decay step size
    uint16_t epsilon_min = 0x0020;          // Minimum epsilon (~0.125)
    uint16_t epsilon_decay_factor = 0x00CC; // Decay factor (~0.8)
    uint8_t epsilon_decay_interval = 10;    // Update every 10 iterations
    uint8_t epsilon_decay_mode = 1;         // Mode 1 (could be linear or exponential)

    // Create register data
    uint64_t rl_regfile_0_data = create_rl_config_regfile_0_data(
        coef_k1, coef_k2, coef_k3, learning_rate);

    uint64_t rl_regfile_1_data = create_rl_config_regfile_1_data(
        discount_factor, initial_epsilon, num_active_actions,
        thres_high, thres_mid, thres_low);

    uint64_t rl_regfile_2_data = create_rl_config_regfile_2_data(
        epsilon_decay_step, epsilon_min, epsilon_decay_factor,
        epsilon_decay_interval, epsilon_decay_mode);

    // Write the data to respective registers
    uint64_t *rl_config_regfile_base_addr = (uint64_t *)0x60000000;
    *(rl_config_regfile_base_addr + 0) = rl_regfile_0_data; // Write to RL regfile 0
    *(rl_config_regfile_base_addr + 1) = rl_regfile_1_data; // Write to RL regfile 1
    *(rl_config_regfile_base_addr + 2) = rl_regfile_2_data; // Write to RL regfile 2
}

void init_standardization_unit(void)
{
    // Base addresses for quantization and dequantization registers
    uint64_t *quant_regfile_base_addr = (uint64_t *)0x60002018;   // Example base address
    uint64_t *dequant_regfile_base_addr = (uint64_t *)0x60002118; // Example base address

    // Number of sensors to configure
    const uint8_t num_sensors = 16; // Example - replace with actual count

    // Configure each sensor's quantization parameters
    for (uint8_t i = 0; i < num_sensors; i++)
    {
        // Example values - replace with actual calibration values
        uint16_t thermal_scale_mult = 0x0100 + i; // Different value per sensor
        uint8_t thermal_scale_shift = i;
        uint8_t thermal_zero_point = 40 + i;

        uint16_t power_scale_mult = 0x0200 + i;
        uint8_t power_scale_shift = 10 + i;
        uint8_t power_zero_point = 20 + i;

        uint64_t quant_data = create_quant_regfile_data(
            thermal_scale_mult, thermal_scale_shift, thermal_zero_point,
            power_scale_mult, power_scale_shift, power_zero_point);

        // Write to the appropriate register for this sensor
        *(quant_regfile_base_addr + i) = quant_data;
    }

    // Configure each sensor's dequantization parameters
    for (uint8_t i = 0; i < num_sensors; i++)
    {
        // Example values - replace with actual calibration values
        uint16_t dequant_scale = 0x00C0 + i;
        uint8_t dequant_shift = 6 + i;
        uint8_t dequant_zero = 120 + i;

        uint64_t dequant_data = create_dequant_regfile_data(
            dequant_scale, dequant_shift, dequant_zero);

        // Write to the appropriate register for this sensor
        *(dequant_regfile_base_addr + i) = dequant_data;
    }
}

void init_q_table(void)
{
    const int NUM_STATES = 512;
    const int ACTIONS_PER_TABLE = 4;
    const uint64_t Q_TABLE_0_BASE_ADDR = 0x60000018; // Replace with actual address
    const uint64_t Q_TABLE_1_BASE_ADDR = 0x60001018; // Replace with actual address

    uint64_t *q_table_0_base = (uint64_t *)Q_TABLE_0_BASE_ADDR;
    uint64_t *q_table_1_base = (uint64_t *)Q_TABLE_1_BASE_ADDR;

    for (int i = 0; i < NUM_STATES; i++)
    {
        uint64_t q_values_0 = 0;
        uint64_t q_values_1 = 0;

        for (int j = 0; j < ACTIONS_PER_TABLE * 2; j++)
        {
            uint16_t q_value;

            // Calculate q_value based on pattern
            switch ((j + i) % 4)
            {
            case 0:
                q_value = 0x0100 + 2 * i + j * 0x0020;
                break;
            case 1:
                q_value = 0xFF00 - i - j * 0x0025;
                break;
            case 2:
                q_value = 0x0000;
                break;
            case 3:
                q_value = 0x0080 + i + j * 0x0010;
                break;
            }

            if (j < ACTIONS_PER_TABLE)
            {
                // Place each action's Q-value in the correct position for table 0
                q_values_0 |= ((uint64_t)q_value << (j * 16));
            }
            else
            {
                // Place each action's Q-value in the correct position for table 1
                q_values_1 |= ((uint64_t)q_value << ((j - ACTIONS_PER_TABLE) * 16));
            }
        }

        // Write values to Q-tables
        q_table_0_base[i] = q_values_0;
        q_table_1_base[i] = q_values_1;
    }
}

void init_tensor_engine_wrapper(void)
{
}

int main()
{
    init_therm_top();
    init_rl_scheduler();
    init_standardization_unit();
    init_q_table();
    // init_tensor_engine_wrapper();
    return 0;
}