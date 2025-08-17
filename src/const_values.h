//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Header file to store constant values used in chip testing.
//////////////////////////////////////////////////////////////////////////////////

#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

#include <stdint.h>

//////////////////////////////////////////////////
// Sensor weight values
// Start from 0x60002238
//////////////////////////////////////////////////
static const int64_t sensor_weight[22] = {

    // I, V, T
    /////////////
    // 25 Hz
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 26903882,
    // -48049234,
    // -517601,
    // 26246,
    // 7445,
    // 721,
    // 149855121,
    // -549385116,
    // 32014350,
    // 494558,
    // -59522,
    // -714

    /////////////
    // 30 Hz
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 26499698,
    // -47448717,
    // -640710,
    // 26076,
    // 9032,
    // 1044,
    // 143393552,
    // -534502159,
    // 37160659,
    // 489047,
    // -70165,
    // -1159,

    /////////////
    // 620.72 MHz
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 26465707,
    // -47497563,
    // -609872,
    // 26207,
    // 9252,
    // 1137,
    // 140119995,
    // -527106150,
    // 39100875,
    // 486372,
    // -75058,
    // -580,

    /////////////
    // 35.063-35.087 Hz
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 26155266,
    // -47098967,
    // -599594,
    // 26163,
    // 10410,
    // 1448,
    // 140565613,
    // -531372861,
    // 44611993,
    // 493391,
    // -86299,
    // -619,

    /////////////
    // 37.009 Hz
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 25734552,
    // -46033777,
    // -680744,
    // 25453,
    // 11138,
    // 1588,
    // 142316907,
    // -539529654,
    // 49080359,
    // 501811,
    // -93627,
    // -930,

    /////////////
    // 37.009 Hz (~1000mV)
    /////////////
    // -45282575,
    // 142440013,
    // -13754022,
    // -1465239,
    // -109520,
    // -4,
    // 1726,
    // 23096,
    // -3296,
    // 1107,
    // 48851588,
    // -110175682,
    // 1470648,
    // 67246,
    // 9746,
    // 1366,
    // 138992857,
    // -491617615,
    // 19722394,
    // 460603,
    // -76289,
    // 6142,

    /////////////
    // 49.788 Hz (~1260mV)
    /////////////
    -45282575,
    142440013,
    -13754022,
    -1465239,
    -109520,
    -4,
    1726,
    23096,
    -3296,
    1107,
    163425402,
    -366334206,
    -41531865,
    181713,
    109090,
    -16782,
    -196651550,
    369895950,
    51034920,
    -72080,
    -140480,
    13700,
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
#define DCO_CONFIG_BASE_ADDR 0x20000000

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