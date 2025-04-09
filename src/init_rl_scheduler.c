//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     C Code Template for RISC-V
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "init_rl_scheduler.h"

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

// Create CSR data for RL Configuration Register 0
uint64_t create_rl_config_regfile_0_data(
    uint16_t coef_k1,
    uint16_t coef_k2,
    uint16_t coef_k3,
    uint16_t learning_rate)
{
    uint64_t data = 0;

    data |= (coef_k1 & 0xFFFFULL);               // 16 bits [15:0]
    data |= ((coef_k2 & 0xFFFFULL) << 16);       // 16 bits [31:16]
    data |= ((coef_k3 & 0xFFFFULL) << 32);       // 16 bits [47:32]
    data |= ((learning_rate & 0xFFFFULL) << 48); // 16 bits [63:48]

    return data;
}

// Create CSR data for RL Configuration Register 1
uint64_t create_rl_config_regfile_1_data(
    uint16_t discount_factor,
    uint16_t initial_epsilon,
    uint8_t num_active_actions,
    uint8_t thres_high,
    uint8_t thres_mid,
    uint8_t thres_low)
{
    uint64_t data = 0;

    data |= (discount_factor & 0xFFFFULL);          // 16 bits [15:0]
    data |= ((initial_epsilon & 0xFFFFULL) << 16);  // 16 bits [31:16]
    data |= ((num_active_actions & 0xFFULL) << 32); // 8 bits [39:32]
    data |= ((thres_high & 0xFFULL) << 40);         // 8 bits [47:40]
    data |= ((thres_mid & 0xFFULL) << 48);          // 8 bits [55:48]
    data |= ((thres_low & 0xFFULL) << 56);          // 8 bits [63:56]

    return data;
}

// Create CSR data for RL Configuration Register 2
uint64_t create_rl_config_regfile_2_data(
    uint16_t epsilon_decay_step,
    uint16_t epsilon_min,
    uint16_t epsilon_decay_factor,
    uint8_t epsilon_decay_interval,
    uint8_t epsilon_decay_mode)
{
    uint64_t data = 0;

    data |= (epsilon_decay_step & 0xFFFFULL);           // 16 bits [15:0]
    data |= ((epsilon_min & 0xFFFFULL) << 16);          // 16 bits [31:16]
    data |= ((epsilon_decay_factor & 0xFFFFULL) << 32); // 16 bits [47:32]
    data |= ((epsilon_decay_interval & 0xFFULL) << 48); // 8 bits [55:48]
    data |= ((epsilon_decay_mode & 0x3ULL) << 56);      // 2 bits [57:56]

    return data;
}