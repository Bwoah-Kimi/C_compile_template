//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "const_values.h"

int main(void) {
    init_uart(104000000, 128000);
    print_uart("Hello RISC-V CPU!\n");

    *(uint32_t*)DCO_CONFIG_BASE_ADDR = 22;        // FC
    *(uint32_t*)(DCO_CONFIG_BASE_ADDR + 1) = 10;  // CC
    *(uint32_t*)(DCO_CONFIG_BASE_ADDR + 3) = 1;   // DIV

    return 0;
}
