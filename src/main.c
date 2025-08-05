//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"

int main(void) {
    init_uart(10000000, 101000);
    print_uart("Hello RISC-V CPU!\n");

    return 0;
}
