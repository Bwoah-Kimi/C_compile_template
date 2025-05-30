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

}
