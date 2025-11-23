// arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -g -nostdlib -nostartfiles -ffreestanding -O0 -Wl,-Map=output.map -Idrivers/stm32 -Idrivers/cmsis -T stm32f303k8.ld startup.s systick.c main.c

#include "thinkernel.h"
#include "stm32f303x8.h"

#define TASK_ONE_ID ( 0x0U )
#define TASK_TWO_ID ( 0x1U )

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
    uint32_t last_toggle = 0;
    uint32_t task1_ctr = 0;

    while (1)
    {
        uint32_t curr_tick = systick_ctr;

        if ((curr_tick - last_toggle) >= 100)
        {
            last_toggle = curr_tick;

            GPIOB->ODR ^= GPIO_ODR_3;

            if (++task1_ctr == 100)
            {
                delay_task(10000);
                task1_ctr = 0;
            }
        }
    }
}

void task2()
{
    uint32_t last_toggle = 0;
    uint32_t task2_ctr = 0;

    while (1)
    {
        uint32_t curr_tick = systick_ctr;

        if ((curr_tick - last_toggle) >= 200)
        {
            last_toggle = curr_tick;

            GPIOB->ODR ^= GPIO_ODR_3;

            if (++task2_ctr == 50)
            {
                delay_task(10000);
                task2_ctr = 0;
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

    create_task(TASK_ONE_ID, 0x8, task1_tcb_ptr, task1_stack, TASK_ONE_STACK_SIZE, task1);
    create_task(TASK_TWO_ID, 0x8, task2_tcb_ptr, task2_stack, TASK_TWO_STACK_SIZE, task2);
    start_thinkernel();
}
