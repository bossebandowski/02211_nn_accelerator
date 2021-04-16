# A simple linker script to put code into the SPM
SECTIONS
{
  . = SEGMENT_START(".rodata", 0x400);
  .rodata : { *(.rodata .rodata.*) }
  .init_array : { *(SORT(.init_array.*) .init_array) }
  .fini_array : { *(SORT(.fini_array.*) .fini_array) }
  .data : { *(.data) }
  .bss : { *(.bss) }

  . = ALIGN(8);
  _end = .; PROVIDE (end = .);

  . = SEGMENT_START(".text", 0x10000);
  .text : { *(.text .text.*) }
}
