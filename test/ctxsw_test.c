
#include "thinkernel.h"

#define TASK_ONE_STACK_SIZE ( 0x200U )
#define TASK_TWO_STACK_SIZE ( 0x200U )

uint32_t task1_stack[TASK_ONE_STACK_SIZE];
Task_t task1_tcb;
Task_t* task1_tcb_ptr = &task1_tcb;

uint32_t task2_stack[TASK_TWO_STACK_SIZE];
Task_t task2_tcb;
Task_t* task2_tcb_ptr = &task2_tcb;

void exit_task()
{
    bool doNothing = true;
    while(doNothing) {};
}

void new_task(Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)())
{
    // Descening stack so location starts at the highest address
    uint32_t* task_stack_loc = stack_addr + stack_size;

    // Make sure the stack location is 32 bit aligned
    task_stack_loc = (uint32_t *)((uint32_t)task_stack_loc & 0xFFFFFFF8U);

    /************************************ Exception Frame ************************************/
    // Need to store exception frame onto stack for a new task.
    *(--task_stack_loc) = 0x01000000U;              // xPSR, execute instructions in thumb state
    *(--task_stack_loc) = (uint32_t)entry_point;    // PC, entry point for this task
    *(--task_stack_loc) = (uint32_t)exit_task;      // LR, exit point for this task

    // Prefill R0, R1, R2, R3, R12
    *(--task_stack_loc) = 0x12121212U;
    *(--task_stack_loc) = 0x03030303U;
    *(--task_stack_loc) = 0x02020202U;
    *(--task_stack_loc) = 0x01010101U;
    *(--task_stack_loc) = 0x00000000U;
    /************************************ Exception Frame ************************************/

    // Store the rest of the registers that aren't in the exception frame onto the stack
    // R4, R5, R6, R7, R8, R9, R10, R11, LR
    *(--task_stack_loc) = 0xFFFFFFFDUL;     // LR, tells the exception handler how to return

    // Prefill R4, R5, R6, R7, R8, R9, R10, R11
    *(--task_stack_loc) = 0x11111111U;
    *(--task_stack_loc) = 0x10101010U;
    *(--task_stack_loc) = 0x09090909U;
    *(--task_stack_loc) = 0x08080808U;
    *(--task_stack_loc) = 0x07070707U;
    *(--task_stack_loc) = 0x06060606U;
    *(--task_stack_loc) = 0x05050505U;
    *(--task_stack_loc) = 0x04040404U;

    task->stack_ptr = task_stack_loc;
}

void ctxsw_test(Task_t* thisTask)
{
    // Set this task as the next highest ready to run task
    highest_prio_task_ptr = thisTask;

    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
}

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
                ctxsw_test(task2_tcb_ptr);
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
                ctxsw_test(task1_tcb_ptr);
            }
        }
    }
}

void main()
{
    init_systick();

    // Set the PendSV interrupt as the lowest priority
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    new_task(task1_tcb_ptr, task1_stack, TASK_ONE_STACK_SIZE, task1);
    new_task(task2_tcb_ptr, task2_stack, TASK_TWO_STACK_SIZE, task2);

    ctxsw_test(task1_tcb_ptr);

    exit_task();
}
