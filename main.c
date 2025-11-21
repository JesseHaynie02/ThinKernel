// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -O0 -Wl,-Map=output.map -Idrivers/stm32 -Idrivers/cmsis -T stm32f303k8.ld startup.s systick.c main.c

#include "thinkernel.h"

#define TASK_ONE_STACK_SIZE ( 0x200U )

uint32_t task1_stack[TASK_ONE_STACK_SIZE];
Task_t task1_tcb;
Task_t* task1_tcb_ptr = &task1_tcb;

void task1()
{
    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    while(1)
    {
        // Polling for systick trigger
        if (systick_ctr > 0)
        {
            // Toggle GPIO PB3 LD3
            GPIOB->ODR ^= GPIO_ODR_3;
            systick_ctr--;
        }
    }
}

void main()
{
    create_task(0x0, 0x8, task1_tcb_ptr, task1_stack, TASK_ONE_STACK_SIZE, task1);
    start_thinkernel();
}
