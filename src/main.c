//////////////////////////////////////////////////////////////////////////////////
// Author:          Zhantong Zhu
// Acknowledgement: GitHub Copilot
// Description:     Main function for RISC-V CPU
//////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "uart.h"
#include "init_config.h"

__attribute__((section(".custom_data")))
static const uint64_t custom_patterns[] = {
    0x0123456789ABCDEF,
    0xFEDCBA9876543210,
    0xAAAABBBBCCCCDDDD,
    0x1111222233334444
};

int main(void) {
    init_uart(10000000, 101000);
    print_uart("Hello RISC-V CPU!\n");

    init_testing();

    return 0;
}
