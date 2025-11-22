// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -O0 -Wl,-Map=output.map -Idrivers/stm32 -Idrivers/cmsis -T stm32f303k8.ld startup.s systick.c main.c

#include "thinkernel.h"
#include "stm32f303x8.h"

#define TASK_ONE_STACK_SIZE ( 0x200U )
#define TASK_TWO_STACK_SIZE ( 0x200U )

uint32_t task1_stack[TASK_ONE_STACK_SIZE];
Task_t task1_tcb;
Task_t* task1_tcb_ptr = &task1_tcb;

uint32_t task2_stack[TASK_TWO_STACK_SIZE];
Task_t task2_tcb;
Task_t* task2_tcb_ptr = &task2_tcb;

// TODO: Remove use of systick counter variable

void task1()
{
    uint32_t task1_ctr = 0;

    while(1)
    {
        if (SysTick->LOAD != 799999U)
        {
            SysTick->LOAD = 799999U;
            SysTick->VAL = 0U;
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                            | SysTick_CTRL_TICKINT_Msk
                            | SysTick_CTRL_ENABLE_Msk;
        }

        // Polling for systick trigger
        if (systick_ctr > 0)
        {
            // Toggle GPIO PB3 LD3
            GPIOB->ODR ^= GPIO_ODR_3;
            systick_ctr--;

            if (++task1_ctr % 100 == 0)
            {
                yield_task(0x0);
            }
        }
    }
}

void task2()
{
    uint32_t task2_ctr = 0;

    while(1)
    {
        if (SysTick->LOAD != 1599999U)
        {
            SysTick->LOAD = 1599999U;
            SysTick->VAL = 0U;
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                            | SysTick_CTRL_TICKINT_Msk
                            | SysTick_CTRL_ENABLE_Msk;
        }

        // Polling for systick trigger
        if (systick_ctr > 0)
        {
            // Toggle GPIO PB3 LD3
            GPIOB->ODR ^= GPIO_ODR_3;
            systick_ctr--;

            if (++task2_ctr % 50 == 0)
            {
                yield_task(0x1);
            }
        }
    }
}

void main()
{
    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    create_task(0x0, 0x8, task1_tcb_ptr, task1_stack, TASK_ONE_STACK_SIZE, task1);
    create_task(0x1, 0x8, task2_tcb_ptr, task2_stack, TASK_TWO_STACK_SIZE, task2);
    start_thinkernel();
}
