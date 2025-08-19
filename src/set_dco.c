//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Set DCO code for RISC-V CPU.
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

int main(void) {

    // 500MHz, VDD=800mV
    // *(uint64_t*)0x20000000 = 10;   // FC
    // *(uint64_t*)0x20000004 = 0;   // CC
    // *(uint64_t*)0x2000000c = 1;   // DIV


    // 600MHz, VDD=920mV
    // *(uint64_t*)0x20000000 = 5;  // FC
    // *(uint64_t*)0x20000004 = 0;  // CC
    // *(uint64_t*)0x2000000c = 1;  // DIV

    // 700MHz, VDD=1000mV
    // *(uint64_t*)0x20000000 = 4;   // FC
    // *(uint64_t*)0x20000004 = 10;  // CC
    // *(uint64_t*)0x2000000c = 1;   // DIV

    // 1GHz, VDD=1260mV
    *(uint64_t*)0x20000000 = 0;   // FC
    *(uint64_t*)0x20000004 = 0;  // CC
    *(uint64_t*)0x2000000c = 1;   // DIV

    return 0;
}
