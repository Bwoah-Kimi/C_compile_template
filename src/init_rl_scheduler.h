#ifndef INIT_RL_SCHEDULER_H
#define INIT_RL_SCHEDULER_H

#include <stdint.h>

void init_rl_scheduler(void);

// Function to create the RL configuration register file 0 data
uint64_t create_rl_config_regfile_0_data(
    uint16_t coef_k1,
    uint16_t coef_k2,
    uint16_t coef_k3,
    uint16_t learning_rate);

// Function to create the RL configuration register file 1 data
uint64_t create_rl_config_regfile_1_data(
    uint16_t discount_factor,
    uint16_t initial_epsilon,
    uint8_t num_active_actions,
    uint8_t thres_high,
    uint8_t thres_mid,
    uint8_t thres_low);

// Function to create the RL configuration register file 2 data
uint64_t create_rl_config_regfile_2_data(
    uint16_t epsilon_decay_step,
    uint16_t epsilon_min,
    uint16_t epsilon_decay_factor,
    uint8_t epsilon_decay_interval,
    uint8_t epsilon_decay_mode);

#endif // INIT_RL_SCHEDULER_H