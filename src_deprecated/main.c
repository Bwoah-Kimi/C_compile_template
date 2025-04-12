//////////////////////////////////////////////////////////////////////////////////
// Author:          Mingxuan Li, Zhantong Zhu
// Acknowledgement: Cursor + Claude
// Description:     C Code Template for RISC-V
//////////////////////////////////////////////////////////////////////////////////

/////////// RISC-V ABI Naming Convention ///////////
// x0  => zero   (always 0)
// x1  => ra     (return address)
// x2  => sp     (stack pointer)
// x3  => gp     (global pointer)
// x4  => tp     (thread pointer)
// x5  => t0     (temporary register 0)
// x6  => t1     (temporary register 1)
// x7  => t2     (temporary register 2)
// x8  => s0/fp  (saved register 0/frame pointer)
// x9  => s1     (saved register 1)
// x10 => a0     (function argument 0)
// x11 => a1     (function argument 1)
// x12 => a2     (function argument 2)
// x13 => a3     (function argument 3)
// x14 => a4     (function argument 4)
// x15 => a5     (function argument 5)
// x16 => a6     (function argument 6)
// x17 => a7     (function argument 7)
// x18 => s2     (saved register 2)
// x19 => s3     (saved register 3)
// x20 => s4     (saved register 4)
// x21 => s5     (saved register 5)
// x22 => s6     (saved register 6)
// x23 => s7     (saved register 7)
// x24 => s8     (saved register 8)
// x25 => s9     (saved register 9)
// x26 => s10    (saved register 10)
// x27 => s11    (saved register 11)
// x28 => t3     (temporary register 3)
// x29 => t4     (temporary register 4)
// x30 => t5     (temporary register 5)
// x31 => t6     (temporary register 6)
/////////////////////////////////////////////////////

#include <stdint.h>

// #include "uart.h"
#include "init_therm_top.h"
#include "init_rl_scheduler.h"
#include "init_standardization_unit.h"
#include "init_q_table.h"

int main()
{
    // init_uart();
    // print_uart("Hello, World!\n");

    init_therm_top();
    init_rl_scheduler();
    init_standardization_unit();
    init_q_table();
    return 0;
}