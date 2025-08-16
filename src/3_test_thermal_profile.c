//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu, Mingxuan Li
// Acknowledgement: GitHub Copilot
// Description:     C code for IVT sensor testing
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "init_config.h"
#include "const_values.h"
#include "d2pu_config.h"

static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}


static inline uint64_t read_hw_cycles(void) {
    return *(volatile uint64_t*)0x60008008;
}


void measure_and_compare_cycles(int iteration, uint8_t read_code, uint8_t read_freq) {
    // Start cycle measurement - both methods
    uint64_t start_cycles = read_cycles();
    uint64_t start_hw_cycles = read_hw_cycles();

    store_sensor_data(
        iteration,
        read_code,
        read_freq
    );

    // End cycle measurement - both methods
    uint64_t end_cycles = read_cycles();
    uint64_t end_hw_cycles = read_hw_cycles();

    uint64_t cycles_taken = end_cycles - start_cycles;
    uint64_t hw_cycles_taken = end_hw_cycles - start_hw_cycles;

    print_uart("Iteration ");
    print_uart_int(iteration);
    print_uart(":\n");
    print_uart("  CPU cycles: ");
    print_uart_addr(cycles_taken);  // Print as hex
    print_uart(" (");
    print_uart_dec(cycles_taken);   // Print as decimal
    print_uart(")\n");
    print_uart("  HW cycles:  ");
    print_uart_addr(hw_cycles_taken);  // Print as hex
    print_uart(" (");
    print_uart_dec(hw_cycles_taken);   // Print as decimal
    print_uart(")\n");

    // Calculate difference
    int64_t diff = (int64_t)cycles_taken - (int64_t)hw_cycles_taken;
    print_uart("  Difference: ");
    if (diff >= 0) {
        print_uart_dec((uint64_t)diff);
    }
    else {
        print_uart("-");
        print_uart_dec((uint64_t)(-diff));
    }
    print_uart(" cycles\n\n");
}


void macro_select(int8_t macro_id, bool end_compute) {
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


__attribute__((section(".d2pu_config_cim_reg_data")))
static const uint64_t config_cim_reg_patterns[] = {
    0x0,
    0x0,
    0x0
};
__attribute__((section(".d2pu_cim_d_reg_data")))
static const uint64_t config_cim_d_patterns[] = {
    0xabd212455231ef13,
    0x1234567890abcdef,
    0xfedcba0987654321,
    0x1122334455667788,
    0x99aabbccddeeff00,
    0x0011223344556677,
    0x8899aabbccddeeff,
    0xdeadbeefdeadbeef,
    0xfeedfacefeedface,
    0xaaaaaaaaaaaaaaaa,
    0x5555555555555555,
    0x0000000000000000,
    0xffffffffffffffff,
    0x0123456789abcdef,
    0xfedcba9876543210,
    0x1234567890abcdef
};
__attribute__((section(".d2pu_cim_ifn_reg_data")))
static const uint64_t config_cim_ifn_patterns[] = {
    0x9876543210fedcba,
    0xc0ffee1234567890,
    0xa1b2c3d4e5f67890,
    0x7f8e9d0c1b2a3948
};


int npu_all_pwr(bool end_compute) {
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

    // Load Func All Instructions
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *(instr_base + 0) = 0;
        *(instr_base + 1) = 0;
    }
    else {
        *(instr_base + 0) = 0b001111100000000000000000000001111;
        *(instr_base + 1) = 0b001110010000000000000000000001111;
    }
    // Execute Func All
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0x0;
    }
    else {
        *instr_config_base = 0b01000000000000001000000001;
    }

    return 0;
}


int npu_func0_pwr(bool end_compute) {
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

    // Load Func0 Instructions
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *instr_base = 0;
    }
    else {
        *instr_base = 0b001111000000000000000000000001111;
    }


    // Execute Func0
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    *instr_config_base = 0b1000000000000000100000001;
    if (end_compute) {
        *instr_config_base = 0;
    }
    else {
        *instr_config_base = 0b1000000000000000100000001;
    }

    return 0;
}


int npu_func1_pwr(bool end_compute) {
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

    // Load Func1 Instructions
    uint64_t* instr_base = (uint64_t*)0x41000000;
    if (end_compute) {
        *instr_base = 0;
    }
    else {
        *instr_base = 0b001110100000000000000000000001111;
    }

    // Execute Func1
    uint64_t* instr_config_base = (uint64_t*)0x42000000;
    if (end_compute) {
        *instr_config_base = 0;
    }
    else {
        *instr_config_base = 0b1000000000000000100000001;
    }

    return 0;
}


int main() {
    init_uart(740000000, 128000);

    print_uart("================================================\n");
    print_uart("Starting Thermal Profiling Test\n");
    print_uart("================================================\n");

    init_power_switch();
    init_sensor_weight();

    // npu_all_pwr(true);
    // npu_func0_pwr(true);
    // npu_func1_pwr(true);
    // macro_select(2, true);

    int power_cycle = 2;
    for (int j = 0; j < power_cycle; j++) {

        // macro_select(9, true);
        // npu_func0_pwr(true);

        uint32_t num_itr = 2000;
        uint32_t sampling_intvl = 1000000;
        init_therm_top(
            1,              // therm_top_start
            1,              // therm_top_en
            0,              // therm_top_stop
            1,              // collect_en
            1,              // collect_mode
            0,              // pred_en
            1,              // pred_mode
            0,              // schedule_en
            1,              // store_sensor_mode
            4,              // action_offset
            num_itr,        // num_itr
            sampling_intvl, // sampling_intvl
            0xA000,         // sensor_data_base_addr
            0xB000,         // pred_data_base_addr
            0xC000,         // action_base_addr
            0x0,            // npu_input_buf_base_addr
            0x8,            // npu_output_buf_base_addr
            0x10,           // synthetic_sensor_thermal_encodings
            0x3,            // synthetic_sensor_current_encodings
            0x2,            // synthetic_sensor_voltage_encodings
            342391,         // synthetic_action_sequence
            0,              // store_pred_mode
            0               // store_action_mode
        );

        for (int i = 0; i < num_itr / 2; i++) {
            // measure_and_compare_cycles(i, 1, 1);
            store_sensor_data(
                i,
                1,  // read_code
                1   // read_freq
            );
        }

        macro_select(9, false);
        // npu_func0_pwr(false);

        for (int i = 0; i < num_itr / 2; i++) {
            // measure_and_compare_cycles(i, 1, 1);
            store_sensor_data(
                i + num_itr / 2,
                1,  // read_code
                1   // read_freq
            );
        }

    }

    return 0;

}