// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -Os -Wl,-Map=output.map -Idrivers/stm32 -Idrivers/cmsis -T stm32f303k8.ld startup.s main.c

#include <stdint.h>

#include "stm32f303x8.h"

#define HSI_VALUE           ( (uint32_t)8000000U )  /* Value of the Internal oscillator in Hz */
#define ONE_THOUSAND_HERTZ  ( (uint32_t)1000U )
#define SYS_TICK_RELOAD     ( (HSI_VALUE / ONE_THOUSAND_HERTZ) - 1 )

void initSysTick()
{
    SysTick->LOAD = SYS_TICK_RELOAD;
    SysTick->VAL = 0U;
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk +
                      SysTick_CTRL_TICKINT_Msk +
                      SysTick_CTRL_TICKINT_Msk);
}

void delay(volatile uint32_t count)
{
    while (count--) __asm__("nop");
}

void main()
{
    initSysTick();

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
