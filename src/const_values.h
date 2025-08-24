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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

// Structure to hold weight coefficients
typedef struct {
    int32_t weightV[6];   // V weights (6 coefficients)
    int32_t weightT[6];   // T weights (6 coefficients)
    int32_t weightI[10];  // I weights (10 coefficients)
} SensorWeights;

static const SensorWeights per_sensor_weights[14] = {
    //SENSOR0
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    35947032,
    -64323933,
    -9299505,
    32822,
    19127,
    2983,
    },
    .weightT = {
    18683780,
    -181097910,
    36987340,
    253617,
    -74679,
    -3111,
    },
    },
    //SENSOR1
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    57246385,
    -112520116,
    -12619825,
    62097,
    18568,
    4183,
    },
    .weightT = {
    115101451,
    -484392494,
    70792818,
    466352,
    -109914,
    -5419,
    },
    },
    //SENSOR2
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    48913286,
    -91619718,
    -12456663,
    50234,
    16903,
    4993,
    },
    .weightT = {
    211995594,
    -749575400,
    92254845,
    647448,
    -144202,
    -2210,
    },
    },
    //SENSOR3
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    166623553,
    -366231248,
    -53907368,
    209415,
    67070,
    4178,
    },
    .weightT = {
    311419643,
    -598920877,
    -209379559,
    459034,
    -67058,
    135600,
    },
    },
    //SENSOR4
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    51919364,
    -102908654,
    -9859780,
    58786,
    14547,
    4705,
    },
    .weightT = {
    96700058,
    -430872772,
    63760107,
    431469,
    -103196,
    -4535,
    },
    },
    //SENSOR5
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    57075251,
    -115539680,
    -11087955,
    67205,
    14691,
    5335,
    },
    .weightT = {
    110594786,
    -466770560,
    71321068,
    450324,
    -105778,
    -7674,
    },
    },
    //SENSOR6
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    44864293,
    -89836128,
    -6984645,
    53954,
    10494,
    5180,
    },
    .weightT = {
    106818788,
    -444002027,
    56258952,
    441902,
    -105045,
    -689,
    },
    },
    //SENSOR7
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    53631647,
    -100379142,
    -16177321,
    52198,
    23761,
    4248,
    },
    .weightT = {
    85753363,
    -401452765,
    64903281,
    405216,
    -91553,
    -10869,
    },
    },
    //SENSOR8
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    55278578,
    -110993121,
    -9689795,
    61907,
    17445,
    3236,
    },
    .weightT = {
    234651251,
    -825405500,
    97534645,
    714815,
    -165518,
    3004,
    },
    },
    //SENSOR9
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    52693260,
    -111713298,
    -5622263,
    68794,
    9403,
    4525,
    },
    .weightT = {
    84732020,
    -382135190,
    48051397,
    393075,
    -80993,
    -6376,
    },
    },
    //SENSOR10
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    45968752,
    -91254724,
    -8557011,
    53803,
    12993,
    4781,
    },
    .weightT = {
    43884952,
    -276066358,
    59039841,
    327100,
    -95968,
    -5212,
    },
    },
    //SENSOR11
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    39040648,
    -70476665,
    -11927455,
    39673,
    15723,
    5639,
    },
    .weightT = {
    -13448072,
    -85624300,
    32929610,
    181794,
    -59830,
    -5638,
    },
    },
    //SENSOR12
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    45243533,
    -88097877,
    -8281941,
    52097,
    10876,
    5362,
    },
    .weightT = {
    142451858,
    -544787044,
    64371373,
    509469,
    -114280,
    -1046,
    },
    },
    //SENSOR13
    {
    .weightI = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    },
    .weightV = {
    39837981,
    -78495752,
    -4775050,
    46378,
    10430,
    3771,
    },
    .weightT = {
    140447845,
    -539215210,
    72467162,
    498467,
    -111578,
    -6300,
    },
    },

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