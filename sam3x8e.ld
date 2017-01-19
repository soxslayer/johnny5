INCLUDE sam3x8e_regs.ld

MEMORY
{
  flash : ORIGIN = 0x80000, LENGTH = 512K
  sram : ORIGIN = 0x20070000, LENGTH = 96K
  nfc_sram : ORIGIN = 0x20100000, LENGTH = 4K
}

SECTIONS
{
  .vector_tbl : {
    . = ALIGN(256);
    __vectortbl_load__ = LOADADDR(.vector_tbl);
    __vectortbl_start__ = .;
    *(.vector_tbl)
    __vectortbl_end__ = .;
  } > sram AT > flash
  .text : { *(.text) } > flash
  .rodata : { *(.rodata .rodata.*) } > flash
  .data : {
    __data_load__ = LOADADDR(.data);
    __data_start__ = .;
    *(.data .data.*)
    . = ALIGN(4);
    __data_end__ = .;
  } > sram AT > flash
  .bss : {
    __bss_start__ = .;
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > sram
  __heap_start__ = .;
  .volcfg : {
    __volcfg_load__ = LOADADDR(.volcfg);
    __volcfg_start__ = .;
    *(.volcfg)
    __volcfg_end__ = .;
  } > nfc_sram AT > flash
}
