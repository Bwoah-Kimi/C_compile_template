#ifndef D2PU_CONFIG_H
#define D2PU_CONFIG_H

#include <stdint.h>
#define SC_CONFIG 0x40600000
#define MODE_CONFIG 0x40f00000
#define INSTR_BASE 0x41000000
#define INSTR_CONFIG 0x42000000
#define IN_SRAM_BASE 0x43000000
#define OUT_SRAM_BASE 0x44000000
#define MED_SRAM_BASE 0x45000000


__attribute__((section(".d2pu_config_cim_reg_data")))
static const uint64_t config_cim_reg_patterns[] = {
    0x0,
    0x0,
    0x0
};

// 16 x 64-bit
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
    // 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

// 4 x 64-bit
__attribute__((section(".d2pu_cim_ifn_reg_data")))
static const uint64_t config_cim_ifn_patterns[] = {
    0x9876543210fedcba,
    0xc0ffee1234567890,
    0xa1b2c3d4e5f67890,
    0x7f8e9d0c1b2a3948
    // 0x0, 0x0, 0x0, 0x0
};



#endif