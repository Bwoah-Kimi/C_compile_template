OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
    . = 0x80000000;

    .text : {
        *(.text.init)
        *(.text)
        *(.text.*)
    }

    .data : {
        . = ALIGN(16);
        *(.data)
        *(.data.*)
    }
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .bss : {
        . = ALIGN(16);
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    . = 0x60002238;
    .sensor_weight : {
        *(.sensor_weight)
        *(.sensor_weight.*)
    }

    . = 0x40d00000;
    .power_switch_code : {
        *(.power_switch_code)
        *(.power_switch_code.*)
    }

}
