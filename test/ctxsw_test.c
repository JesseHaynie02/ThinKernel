
#include "thinkernel.h"

#define TASK_ONE_STACK_SIZE ( 0x400U )

uint32_t task1_stack[TASK_ONE_STACK_SIZE];
Task_t task1_tcb;
Task_t* task1_tcb_ptr = &task1_tcb;

void task1()
{
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

void exit_task()
{
    bool doNothing = true;
    while(doNothing) {};
}

void ctxsw_test()
{
    // Descening stack so location starts at the highest address
    uint32_t* task1_stck_loc = task1_stack + TASK_ONE_STACK_SIZE;

    // Make sure the stack location is 32 bit aligned
    task1_stck_loc = (uint32_t *)((uint32_t)task1_stck_loc & 0xFFFFFFF8U);

    /************************************ Exception Frame ************************************/
    // Need to store exception frame onto stack for a new task.
    *(--task1_stck_loc) = 0x01000000U;          // xPSR, execute instructions in thumb state
    *(--task1_stck_loc) = (uint32_t)task1;      // PC, entry point for this task
    *(--task1_stck_loc) = (uint32_t)exit_task;  // LR, exit point for this task

    // Prefill R0, R1, R2, R3, R12
    *(--task1_stck_loc) = 0x12121212U;
    *(--task1_stck_loc) = 0x03030303U;
    *(--task1_stck_loc) = 0x02020202U;
    *(--task1_stck_loc) = 0x01010101U;
    *(--task1_stck_loc) = 0x00000000U;
    /************************************ Exception Frame ************************************/

    // Store the rest of the registers that aren't in the exception frame onto the stack
    // R4, R5, R6, R7, R8, R9, R10, R11, LR
    *(--task1_stck_loc) = 0xFFFFFFFDUL;     // LR, tells the exception handler how to return

    // Prefill R4, R5, R6, R7, R8, R9, R10, R11
    *(--task1_stck_loc) = 0x11111111U;
    *(--task1_stck_loc) = 0x10101010U;
    *(--task1_stck_loc) = 0x09090909U;
    *(--task1_stck_loc) = 0x08080808U;
    *(--task1_stck_loc) = 0x07070707U;
    *(--task1_stck_loc) = 0x06060606U;
    *(--task1_stck_loc) = 0x05050505U;
    *(--task1_stck_loc) = 0x04040404U;

    // Set this task as the next highest ready to run task
    highest_prio_task_ptr = task1_tcb_ptr;
    task1_tcb_ptr->stack_ptr = task1_stck_loc;

    // Set the PendSV interrupt as the lowest priority
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
    while(1) {};
}

void main()
{
    init_systick();

    // Enable GPIO B Port
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Reset GPIO B Port 3 then set as output
    GPIOB->MODER &= ~(GPIO_MODER_MODER3);
    GPIOB->MODER |= GPIO_MODER_MODER3_0;

    ctxsw_test();
    exit_task();
}
