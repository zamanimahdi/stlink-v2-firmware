/* Linker script for the decrypted updated ST-LINK/V2 image (STM32F103, 128 KB flash).
   Layout of the update payload: signature block, application, then two trailing data blobs. */
ENTRY(APP_Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08003C00, LENGTH = 0x1C400
}

SECTIONS
{
  .sig 0x08003C00 : { KEEP(*(.sig)) } > FLASH
  ASSERT(SIZEOF(.sig) == 0x400, "signature block must be 1 KB")

  .app_image 0x08004000 : { KEEP(*(.app_image)) } > FLASH

  .blob1 0x0801F800 : { KEEP(*(.blob1)) } > FLASH
  .blob2 0x0801FFF0 : { KEEP(*(.blob2)) } > FLASH

  /DISCARD/ : { *(.text*) *(.data*) *(.bss*) }
}
