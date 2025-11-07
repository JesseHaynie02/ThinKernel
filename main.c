// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -Os -Wl,-Map=output.map -Idrivers/stm32 -Idrivers/cmsis -T stm32f303k8.ld startup.s main.c

#include <stdint.h>

#include "stm32f303x8.h"

void delay(volatile uint32_t count)
{
    while (count--) __asm__("nop");
}

void main()
{
    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    while(1)
    {
        // Toggle GPIO PB3 LD3
        GPIOB->ODR ^= GPIO_ODR_3;
        delay(500000);
    }
}
