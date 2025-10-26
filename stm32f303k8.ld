/*
*  Linker script for NUCLEO-F303K8 Board embedding STM32F303K8Tx Device from stm32f3 series.
*
*  ARMv7E-M Cortex-M4
*  64KBytes FLASH
*  4KBytes CCMRAM
*  12KBytes RAM
*/

/* The MEMORY command describes the location and size of blocks of memory in the target. */
MEMORY
{
    /* name [(attr)] : ORIGIN = origin , LENGTH = len */
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    CCMRAM (rwx) : ORIGIN = 0x1000000, LENGTH = 4K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 12K
}

ENTRY(reset_handler)
