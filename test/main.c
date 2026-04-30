#include "yield_task_test.h"

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

    create_semaphore(SEMA_ONE_ID, sema1_ptr, 0);
    create_semaphore(SEMA_TWO_ID, sema2_ptr, 1);

    create_task(TASK_ONE_ID, 0x8, task1_tcb_ptr, task1_stack, TASK_ONE_STACK_SIZE, task1);
    create_task(TASK_TWO_ID, 0x8, task2_tcb_ptr, task2_stack, TASK_TWO_STACK_SIZE, task2);

    start_thinkernel();
}
