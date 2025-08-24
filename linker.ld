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

    . = 0x40000000;
    .d2pu_config_cim_reg_data : {
        *(.d2pu_config_cim_reg_data)
        *(.d2pu_config_cim_reg_data.*)
    }

    . = 0x40200000;
    .d2pu_cim_d_reg_data : {
        *(.d2pu_cim_d_reg_data)
        *(.d2pu_cim_d_reg_data.*)
    }

    . = 0x40400000;
    .d2pu_cim_ifn_reg_data : {
        *(.d2pu_cim_ifn_reg_data)
        *(.d2pu_cim_ifn_reg_data.*)
    }

}
