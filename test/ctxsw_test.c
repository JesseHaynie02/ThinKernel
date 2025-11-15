
#include "thinkernel.h"

// Task struct to emulate a real task
// except this one just holds the stack pointer
// to test context switching.
typedef struct
{
    uint32_t* stck_ptr;
} Tasktst_t;

Tasktst_t* curr_task_ptr;
Tasktst_t* highest_prio_task_ptr;

uint32_t task1_stack[0x400];
Tasktst_t task1_tcb;
Tasktst_t* task1_tcb_ptr = &task1_tcb;

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
    uint32_t* task1_stck_loc = task1_stack + 0x400;

    uint32_t* stack_ptr = task1_stck_loc;
    stack_ptr = (uint32_t *)((uint32_t)stack_ptr & 0xFFFFFFF8U);  // 8-byte align

    // cortex-m4 uses descending stack. Need to store exception frame onto stack for a new task.
    // R0, R1, R2, R3, R12, LR, PC, xPSR
    *(--stack_ptr) = 0x01000000U;
    *(--stack_ptr) = (uint32_t)task1;
    *(--stack_ptr) = (uint32_t)exit_task;
    *(--stack_ptr) = 0x12121212U;
    *(--stack_ptr) = 0x03030303U;
    *(--stack_ptr) = 0x02020202U;
    *(--stack_ptr) = 0x01010101U;
    *(--stack_ptr) = 0x00000000U;

    *(--stack_ptr) = 0xFFFFFFFDUL;
    *(--stack_ptr) = 0x11111111U;
    *(--stack_ptr) = 0x10101010U;
    *(--stack_ptr) = 0x09090909U;
    *(--stack_ptr) = 0x08080808U;
    *(--stack_ptr) = 0x07070707U;
    *(--stack_ptr) = 0x06060606U;
    *(--stack_ptr) = 0x05050505U;
    *(--stack_ptr) = 0x04040404U;

    task1_tcb_ptr->stck_ptr = stack_ptr;

    NVIC_SetPriority(PendSV_IRQn, 15);
    highest_prio_task_ptr = task1_tcb_ptr;
    curr_task_ptr = NULL;

    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // PendSV pending
    __DSB();
    __ISB();
    while(1); // Wait for context switch
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
