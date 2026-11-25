/* Production all-RAM linker script for STM32H750IB.
 * Stores the full firmware image in flash (LMA), executes from AXI SRAM (VMA).
 * A tiny flash-resident boot stub (.isr_vector + .flash_boot) copies the image
 * flash→SRAM and jumps; that stub is the only code ever executed from flash.
 */

MEMORY
{
    FLASH    (RX)  : ORIGIN = 0x08000000, LENGTH = 128K
    DTCMRAM  (RWX) : ORIGIN = 0x20000000, LENGTH = 100K
    SRAM     (RWX) : ORIGIN = 0x24000000, LENGTH = 512K
    RAM_D2   (RWX) : ORIGIN = 0x30000000, LENGTH = 288K
    RAM_D3   (RWX) : ORIGIN = 0x38000000, LENGTH = 64K
    SDRAM    (RWX) : ORIGIN = 0xc0000000, LENGTH = 64M
}

_estack = 0x20020000;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .flash_boot :
    {
        . = ALIGN(4);
        KEEP(*(.flash_boot))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        _ssram = .;
        _stext = .;

        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;

    } > SRAM AT > FLASH

    .ARM.extab :
    {
        . = ALIGN(4);
        *(.ARM.extab)
        *(.gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > SRAM AT > FLASH

    .exidx :
    {
        . = ALIGN(4);
        PROVIDE(__exidx_start = .);
        *(.ARM.exidx*)
        . = ALIGN(4);
        PROVIDE(__exidx_end = .);
    } > SRAM AT > FLASH

    .ARM.attributes :
    {
        *(.ARM.attributes)
    } > SRAM AT > FLASH

    .preinit_array :
    {
        PROVIDE(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE(__preinit_array_end = .);
    } > SRAM AT > FLASH

    .init_array :
    {
        PROVIDE(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE(__init_array_end = .);
    } > SRAM AT > FLASH

    .fini_array :
    {
        PROVIDE(__fini_array_start = .);
        KEEP(*(.fini_array*))
        KEEP(*(SORT(.fini_array.*)))
        PROVIDE(__fini_array_end = .);
        _esram = .;
    } > SRAM AT > FLASH

    _sram_image_lma = LOADADDR(.text);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;

        PROVIDE(__data_start__ = _sdata);
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;

        PROVIDE(__data_end__ = _edata);
    } > DTCMRAM AT > FLASH

    _sidata = LOADADDR(.data);

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;

        PROVIDE(__bss_start__ = _sbss);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;

        PROVIDE(__bss_end__ = _ebss);
    } > DTCMRAM

    PROVIDE(end = .);
}

ASSERT(SIZEOF(.isr_vector) > 0, "no boot vectors: include <mcu/stm/h750/mcu_h750_sram_flash.h> in the .cpp that has main()")
