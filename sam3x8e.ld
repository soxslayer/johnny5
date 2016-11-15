MEMORY
{
  flash : ORIGIN = 0x80000, LENGTH = 512K
  sram : ORIGIN = 0x20070000, LENGTH = 96K
}

SECTIONS
{
  .vector_tbl : { *(.vector_tbl) } > flash
  .text : { *(.text) } > flash
  .rodata : { *(.rodata .rodata.*) } > flash
  .data : {
    __data_load__ = LOADADDR(.data);
    __data_start__ = .;
    *(.data .data.*)
    . = ALIGN(4);
    __data_end__ = .;
  } > sram AT>flash
  .bss : {
    __bss_start__ = .;
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > sram
}
