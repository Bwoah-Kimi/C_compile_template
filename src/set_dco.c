//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Set DCO code for RISC-V CPU.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

int main(void) {

    // 500MHz, VDD=800mV
    // *(uint64_t*)0x20000000 = 4;   // FC
    // *(uint64_t*)0x20000004 = 5;   // CC
    // *(uint64_t*)0x2000000c = 1;   // DIV


    // 600MHz, VDD=920mV
    // *(uint64_t*)0x20000000 = 5;  // FC
    // *(uint64_t*)0x20000004 = 0;  // CC
    // *(uint64_t*)0x2000000c = 1;  // DIV

    // 700MHz, VDD=1040mV
    *(uint64_t*)0x20000000 = 8;   // FC
    *(uint64_t*)0x20000004 = 3;  // CC
    *(uint64_t*)0x2000000c = 1;   // DIV

    // 900MHz, VDD=1100mV
    // *(uint64_t*)0x20000000 = 0;   // FC
    // *(uint64_t*)0x20000004 = 5;  // CC
    // *(uint64_t*)0x2000000c = 1;   // DIV


    // 1GHz, VDD=1200mV
    // *(uint64_t*)0x20000000 = 0;  // FC
    // *(uint64_t*)0x20000004 = 0;  // CC
    // *(uint64_t*)0x2000000c = 1;  // DIV

    // 1.1GHz, VDD=1300mV
    // *(uint64_t*)0x20000000 = 37;  // FC
    // *(uint64_t*)0x20000004 = 45;  // CC
    // *(uint64_t*)0x2000000c = 2;  // DIV
    return 0;
}
