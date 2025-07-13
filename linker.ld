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

    . = 0x30000000;
    .custom_data : {
        *(.custom_data)
        *(.custom_data.*)
    }

}
