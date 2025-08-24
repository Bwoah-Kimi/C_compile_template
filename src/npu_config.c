//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu, Mingxuan Li
// Acknowledgement: GitHub Copilot
// Description:     C code for NPU configurations
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "const_values.h"
#include "d2pu_config.h"
#include "store_data.h"
#include "npu_config.h"


// Simple Linear Congruential Generator (LCG) for pseudorandom numbers
static uint32_t prng_seed = 1;

void set_random_seed(uint32_t seed) {
    prng_seed = seed;
}


uint32_t simple_rand() {
    // LCG parameters (same as used in glibc)
    prng_seed = (prng_seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return prng_seed;
}


uint64_t rand64() {
    // Generate 64-bit random number by combining two 32-bit values
    uint64_t high = (uint64_t)simple_rand() << 32;
    uint64_t low = (uint64_t)simple_rand();
    return high | low;
}


void change_input_data(uint8_t data_id, int start_idx, int end_idx) {
    // Change MODE_CONFIG to SRAM Read/Write mode
    *((volatile uint64_t*)MODE_CONFIG) = 0x100;

    // Write SRAM with different data sparsity
    uint64_t* input_sram_base = (uint64_t*)IN_SRAM_BASE;

    if (data_id == 0) {
        // Random input
        for (int i = start_idx; i < end_idx; i++) {
            *(input_sram_base + i) = rand64();
        }
    }
    else if (data_id == 1) {
        // All ONE input
        for (int i = start_idx; i < end_idx; i++) {
            *(input_sram_base + i) = 0xFFFFFFFFFFFFFFFF;
        }
    }
    else if (data_id == 2) {
        // All ZERO input
        for (int i = start_idx; i < end_idx; i++) {
            *(input_sram_base + i) = 0x0;
        }
    }
    else if (data_id == 3) {
        // A5A5 input
        for (int i = start_idx; i < end_idx; i++) {
            *(input_sram_base + i) = 0xA5A5A5A5A5A5A5A5;
        }
    }
    else if (data_id == 4) {
        // Interleaved input
        for (int i = start_idx; i < end_idx; i++) {
            if (i % 2 == 0) {
                *(input_sram_base + i) = 0xFFFFFFFFFFFFFFFF;
            }
            else {
                *(input_sram_base + i) = 0xABABABABABABABAB;
            }
        }
    }
    else if (data_id == 5) {
        // Interleaved input
        for (int i = start_idx; i < end_idx; i++) {
            if (i % 2 == 0) {
                *(input_sram_base + i) = 0xA5A5A5A5A5A5A5A5;
            }
            else {
                *(input_sram_base + i) = 0x0;
            }
        }
    }

    // Change MODE_CONFIG to power mode
    *((volatile uint64_t*)MODE_CONFIG) = 0x0;
}


void change_med_data(uint8_t data_id, int start_idx, int end_idx) {
    // Change MODE_CONFIG to SRAM Read/Write mode
    *((volatile uint64_t*)MODE_CONFIG) = 0x100;

    // Write SRAM with different data sparsity
    uint64_t* med_sram_base = (uint64_t*)MED_SRAM_BASE;
    if (data_id == 0) {
        // Random input
        for (int i = start_idx; i < end_idx; i++) {
            *(med_sram_base + i) = rand64();
        }
    }
    else if (data_id == 1) {
        // All ONE input
        for (int i = start_idx; i < end_idx; i++) {
            *(med_sram_base + i) = 0xFFFFFFFFFFFFFFFF;
        }
    }
    else if (data_id == 2) {
        // All ZERO input
        for (int i = start_idx; i < end_idx; i++) {
            *(med_sram_base + i) = 0x0;
        }
    }
    else if (data_id == 3) {
        // A5A5 input
        for (int i = start_idx; i < end_idx; i++) {
            *(med_sram_base + i) = 0xA5A5A5A5A5A5A5A5;
        }
    }
    else if (data_id == 4) {
        // Interleaved input
        for (int i = start_idx; i < end_idx; i++) {
            if (i % 2 == 0) {
                *(med_sram_base + i) = 0xFFFFFFFFFFFFFFFF;
            }
            else {
                *(med_sram_base + i) = 0xABABABABABABABAB;
            }
        }
    }
    else if (data_id == 5) {
        // Interleaved input
        for (int i = start_idx; i < end_idx; i++) {
            if (i % 2 == 0) {
                *(med_sram_base + i) = 0xA5A5A5A5A5A5A5A5;
            }
            else {
                *(med_sram_base + i) = 0x0;
            }
        }
    }

    // Change MODE_CONFIG to power mode
    *((volatile uint64_t*)MODE_CONFIG) = 0x0;
}


void read_macro_data() {
    //// 写 CIM 1 的 0 地址，并读到 output sram 0 地址 ////
    // 写 CIM 1 的 0 地址
    *((volatile uint64_t*)MODE_CONFIG) = 0x100 | 0x001;
    *(volatile uint64_t*)INSTR_BASE = 0b100001000000000000000000000000001;
    *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;

    // 读 CIM 1 的 0 地址到 output sram 0 地址
    *((volatile uint64_t*)MODE_CONFIG) = 0x100 | 0x001;
    *(volatile uint64_t*)INSTR_BASE = 0b100100000000000000000000000000001;
    *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;

    //// 写 CIM 1 的 1 地址，并读到 output sram 1 地址 ////
    // 写 CIM 1 的 1 地址
    *((volatile uint64_t*)MODE_CONFIG) = 0x001;
    *(volatile uint64_t*)INSTR_BASE = 0b100001000000000000000000000000001 | (1 << 18);
    *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;

    // 读 CIM 1 的 1 地址到 output sram 1 地址
    *((volatile uint64_t*)MODE_CONFIG) = 0x001;
    *(volatile uint64_t*)INSTR_BASE = 0b100100000000000000000000000000001 | (1 << 18) | (1 << 6);
    *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;

    //// 写 CIM 1 的 所有地址，并读到 output sram 对应地址 ////
    // 写 CIM 1 的 所有地址
    *((volatile uint64_t*)MODE_CONFIG) = 0x002;
    for (int i = 0; i < 512; i++)
    {
        *((volatile uint64_t*)INSTR_BASE + i) = 0b100001000000000000000000000000001 | (i << 18);
        *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;
    }
    // 读 CIM 1 的 所有地址到 output sram 对应地址
    for (int i = 0; i < 512; i++)
    {
        // Load instruction
        *((volatile uint64_t*)INSTR_BASE + i) = 0b100100000000000000000000000000001 | (i << 18) | (i << 6);
        *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;
    }
}


void change_macro_data(uint8_t data_id, uint8_t macro_id) {
    // Write first 4 lines of input SRAM
    change_input_data(data_id, 0, 4);

    // Write selected macro
    *((volatile uint64_t*)MODE_CONFIG) = data_id;
    for (int i = 0; i < 512; i++) {
        // Load instruction
        *((volatile uint64_t*)INSTR_BASE) = 0b000001000000000000000000000000001 | (i << 16);
        *((volatile uint64_t*)INSTR_CONFIG) = 0x0101;
    }

    // Change MODE_CONFIG to power mode
    *((volatile uint64_t*)MODE_CONFIG) = 0x0;
}


void change_npu_func(uint8_t func_id) {
    uint64_t* instr_base = (uint64_t*)0x41000000;
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (func_id == 0) {
        // Load Func0 Instructions
        *instr_base = 0b001111000000000000000000000001111;
    }
    else if (func_id == 1) {
        // Load Func1 Instructions
        *instr_base = 0b001110100000000000000000000001111;
    }
    else if (func_id == 2) {
        // Load Func2 Instructions
        *instr_base = 0b000110010000000000000000000001111;
    }
    // Execute Func
    *((volatile uint64_t*)INSTR_CONFIG) = 0b1000000000000000100000001;

    return;
}


void change_npu_task(uint8_t task_id, bool end_compute) {
    if (end_compute) {
        *((volatile uint64_t*)INSTR_CONFIG) = 0x0;
        return;
    }
    if (task_id == 0) {
        change_input_data(0, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 1) {
        change_input_data(1, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 2) {
        change_input_data(2, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 3) {
        change_input_data(3, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 4) {
        change_input_data(4, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 5) {
        change_input_data(5, 0, 4608);
        change_npu_func(0);
    }
    else if (task_id == 6) {
        change_input_data(0, 0, 1536);
        change_input_data(2, 1537, 3072);
        change_input_data(2, 3073, 4608);
        change_npu_func(0);
    }
    else if (task_id == 7) {
        change_input_data(2, 0, 1536);
        change_input_data(2, 1537, 3072);
        change_input_data(0, 3073, 4608);
        change_npu_func(0);
    }
    else if (task_id == 8) {
        change_input_data(2, 0, 1536);
        change_input_data(0, 1537, 3072);
        change_input_data(2, 3073, 4608);
        change_npu_func(0);
    }
    else if (task_id == 9) {
        change_npu_func(1);
    }
    else if (task_id == 10) {
        change_med_data(0, 0, 1152);
        change_npu_func(2);
    }
    else if (task_id == 11) {
        change_med_data(2, 0, 1152);
        change_npu_func(2);
    }
}


void sync_npu(bool change_task, uint8_t task_id, bool end_compute, uint32_t itr, uint32_t sensor_data_base_addr) {
    // Wait for the CPU start flag
    while ((*(volatile uint64_t*)CPU_START_FLAG_ADDR) != CPU_STORE_SENSOR_START_FLAG) {}

    // Process sensor data
    process_sensor_data(itr, sensor_data_base_addr);

    if (change_task) {
        change_npu_task(task_id, end_compute);
    }

    // Set finish flag
    *(volatile uint64_t*)CPU_FINISH_FLAG_ADDR = CPU_STORE_SENSOR_FINISH_FLAG;
    return;
}


void macro_select(uint8_t macro_id, bool end_compute) {
    // scan chain config
    *((volatile uint64_t*)SC_CONFIG) = 0b101;
    while (1) {
        if ((*((volatile uint64_t*)SC_CONFIG) & 0xff00) == 0)
        {
            break;
        }
    }

    *((volatile uint64_t*)SC_CONFIG) = 0b110;
    while (1) {
        if ((*((volatile uint64_t*)SC_CONFIG) & 0xff00) == 0)
        {
            break;
        }
    }

    *((volatile uint64_t*)SC_CONFIG) = 0b111;
    while (1) {
        if ((*((volatile uint64_t*)SC_CONFIG) & 0xff00) == 0)
        {
            break;
        }
    }

    // config instr

    // Mode Select
    // [7:0]: (macro select) 1,3,5,7,9,10,11 => CIM; 2,4,6,8 => PE
    // [8]:   1'b1 => debug mode; 1'b0 => func mode
    uint64_t mode_config = 0x100 | macro_id;
    *((volatile uint64_t*)MODE_CONFIG) = mode_config;

    ////////////////////////////////////////////////////////
    /*-----------------Debug Instruction format ---------------*/
    // loop_op      <= instruction[31]   ;
    // cen          <= instruction[30]   ;
    // ren_global   <= instruction[29]   ;
    // ren_local    <= instruction[28]   ;
    // wen          <= instruction[27]   ;
    // cim_addr     <= instruction[26:18];
    // gbuffer_addr <= instruction[17:6] ;
    // op_cycle     <= instruction[5:0]  ;
    ////////////////////////////////////////////////////////
    uint64_t instr;
    instr = 0b101111000000000000000000000001111;
    *((volatile uint64_t*)INSTR_BASE) = instr;

    // Execute instructions
    // [0]:     load
    // [13:8]:  instr num
    // [16]:    sram (in/out)
    // [24]:    loop
    if (end_compute) {
        *((volatile uint64_t*)INSTR_CONFIG) = 0x0;
    }
    else {
        *((volatile uint64_t*)INSTR_CONFIG) = 0x1000101;
    }

    return;
}


void npu_all_pwr(bool end_compute) {
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *(instr_base + 0) = 0;
        *(instr_base + 1) = 0;
    }
    else {
        // Load Func All Instructions
        *(instr_base + 0) = 0b001111100000000000000000000001111;
        *(instr_base + 1) = 0b001110010000000000000000000001111;
    }
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0x0;
    }
    else {
        // Execute Func All
        *instr_config_base = 0b01000000000000001000000001;
    }

    return;
}


void npu_func0_pwr(bool end_compute) {
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *instr_base = 0;
    }
    else {
        // Load Func0 Instructions
        *instr_base = 0b001111000000000000000000000001111;
    }
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0;
    }
    else {
        // Execute Func0
        *instr_config_base = 0b1000000000000000100000001;
    }
    return;
}


void npu_func1_pwr(bool end_compute) {
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *instr_base = 0;
    }
    else {
        // Load Func1 Instructions
        *instr_base = 0b001110100000000000000000000001111;
    }

    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0;
    }
    else {
        // Execute Func1
        *instr_config_base = 0b1000000000000000100000001;
    }
    return;
}


void npu_func2_pwr(bool end_compute) {
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *instr_base = 0;
    }
    else {
        // Load Func2 Instructions
        *(instr_base + 0) = 0b000110010000000000000000000001111;
        *(instr_base + 1) = 0b001111100000000000000000000001111;
    }
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0;
    }
    else {
        // Execute Func2
        *instr_config_base = 0b1000000000000000100000001;
    }
}


void init_cim_config() {
    // Load CIM Configuration
    uint64_t* cim_config_ctrl_base = (uint64_t*)0x40600000;
    *cim_config_ctrl_base = 0b101;
    for (int i = 0; i < 100; i++);
    *cim_config_ctrl_base = 0b0;

    // Load CIM D Registers
    *cim_config_ctrl_base = 0b110;
    for (int i = 0; i < 100; i++);
    *cim_config_ctrl_base = 0b0;

    // Load CIM IFN Registers
    *cim_config_ctrl_base = 0b111;
    for (int i = 0; i < 100; i++);
    *cim_config_ctrl_base = 0b0;

    return;
}


void init_macro_data() {
    for (uint8_t i = 1; i <= 12; i++) {
        change_macro_data(0, i);
    }
}