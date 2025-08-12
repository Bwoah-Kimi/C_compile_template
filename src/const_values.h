//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Header file to store constant values used in chip testing.
//////////////////////////////////////////////////////////////////////////////////

#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

#include <stdint.h>

//////////////////////////////////////////////////
// Power switch configurations
// 14 power switches, each 8-bit configuration
//////////////////////////////////////////////////
static const uint8_t power_switch_config[14] = {
    0b00100000,  // Power switch 0
    0b00100000,  // Power switch 1
    0b00100000,  // Power switch 2
    0b00100000,  // Power switch 3
    0b00100000,  // Power switch 4
    0b00100000,  // Power switch 5
    0b00100000,  // Power switch 6
    0b00100000,  // Power switch 7
    0b00100000,  // Power switch 8
    0b00100000,  // Power switch 9
    0b00100000,  // Power switch 10
    0b00100000,  // Power switch 11
    0b00100000,  // Power switch 12
    0b00100000   // Power switch 13
};

// #define NUM_POWER_SWITCHES 14

//////////////////////////////////////////////////
// Sensor weight values
// Start from 0x60002238
//////////////////////////////////////////////////
static const int64_t sensor_weight[22] = {
    // Current calculation weights [0-9] (10 x 64-bit values)
    1120538,      // Current weight 0
    -3316961,     // Current weight 1
    578850,       // Current weight 2
    -472836,      // Current weight 3
    2391,         // Current weight 4
    246,          // Current weight 5
    81,           // Current weight 6
    -951,         // Current weight 7
    -196,         // Current weight 8
    809,          // Current weight 9

    // Voltage calculation weights [10-15] (6 x 64-bit values)
    0b00000000111011001010111000000110,   // Voltage weight 0
    0b11111111001001001101100101101000,   // Voltage weight 1
    0b11111111111000000000000111001011,   // Voltage weight 2
    0b00000000000000000000010010010010,   // Voltage weight 3
    0b00000000000000000010011111101100,   // Voltage weight 4
    0b00000000000000000000010010111001,   // Voltage weight 5

    // Thermal calculation weights [16-21] (6 x 64-bit values)
    0b00000110001110011001111100101101,   // Thermal weight 0
    0b11100101110010001011001110111110,   // Thermal weight 1
    0b00000010010011011111100011111101,   // Thermal weight 2
    0b00000000000001101100110000100100,   // Thermal weight 3
    0b11111111111111101110001010111101,   // Thermal weight 4
    0b11111111111111111111111110111000    // Thermal weight 5
};

////////////////////////////////////////////////
// Address Mapping Definitions
////////////////////////////////////////////////
#define TOP_CONFIG_REGFILE_BASE_ADDR  0x60002218

#define RL_CONFIG_REGFILE_BASE_ADDR 0x60000000

#define Q_TABLE_0_BASE_ADDR 0x60000018
#define Q_TABLE_1_BASE_ADDR 0x60001018

#define QUANT_CONFIG_REGFILE_BASE_ADDR 0x60002018
#define DEQUANT_CONFIG_REGFILE_BASE_ADDR 0x60002118

#define SENSOR_WEIGHT_BUFFER_BASE_ADDR 0x60002238
#define SENSOR_FREQ_BUFFER_BASE_ADDR 0x60002338
#define SENSOR_ENCODINGS_BASE_ADDR 0x60005000
#define SENSOR_PRED_BASE_ADDR 0x60006000

#define RL_ACTION_BASE_ADDR 0x60006100

#define CPU_START_FLAG_ADDR 0x60007000
#define CPU_FINISH_FLAG_ADDR 0x60007008

#define CPU_COMPUTE_PERF_COUNTER_ADDR 0x60008000
#define CPU_STORE_SENSOR_PERF_COUNTER_ADDR 0x60008008
#define CPU_STORE_PRED_PERF_COUNTER_ADDR 0x60008010
#define CPU_STORE_ACTION_PERF_COUNTER_ADDR 0x60008018

#define POWER_SWITCH_BASE_ADDR 0x40d00000

#define CPU_COMPUTE_START_FLAG 0x00000000A5A5A5A5
#define CPU_COMPUTE_FINISH_FLAG 0x5A5A5A5A00000000
#define CPU_STORE_SENSOR_START_FLAG 0x00000000B1B1B1B1
#define CPU_STORE_SENSOR_FINISH_FLAG 0x1B1B1B1B00000000
#define CPU_STORE_PRED_START_FLAG 0x00000000C2C2C2C2
#define CPU_STORE_PRED_FINISH_FLAG 0x2C2C2C2C00000000
#define CPU_STORE_ACTION_START_FLAG 0x00000000D3D3D3D3
#define CPU_STORE_ACTION_FINISH_FLAG 0x3D3D3D3D00000000

////////////////////////////////////////////////
// THERM_TOP Constants
////////////////////////////////////////////////

#define NUM_STATES 512
#define ACTION_PER_TABLE 4

#endif // ADDRESS_MAP_H