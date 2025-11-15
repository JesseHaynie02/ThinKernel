
#include <stdint.h>
#include <stdbool.h>

#include "core_cm4.h"

Tasktst_t* highest_prio_task_ptr;

typedef struct
{
    uint32_t* stck_ptr;
} Tasktst_t;

void ctxsw_test()
{
    // Allocate space on the heap for a stack of a particular thread
    uint32_t* task1_stack = (uint32_t *)malloc(0x400 * sizeof(uint32_t));
    // uint32_t* task2_stack = (uint32_t *)malloc(0x400 * sizeof(uint32_t));

    Tasktst_t* task1_tcb = (Tasktst_t *)malloc(sizeof(Tasktst_t));

    uint32_t* task1_stck_loc = task1_stack + 0x400;
    // uint32_t task2_stck_loc = task2_stack + 0x400;

    // cortex-m4 uses descending stack. Need to store exception frame onto stack for a new task.
    // R0, R1, R2, R3, R12, LR, PC, xPSR
    uint32_t* stack_ptr = task1_stck_loc;

    stack_ptr -= 8;                     // Decrease stack pointer by 32 bytes
    stack_ptr[0] = 0x01000000;          // xPSR, set T-bit specifies Thumb mode
    stack_ptr[1] = (uint32_t)&task1;     // Set PC to entry point
    stack_ptr[2] = (uint32_t)&exit;      // Set LR to exit funtion to idle
    stack_ptr[3] = 0;                   // Fill zeroes in R0-R3 R12
    stack_ptr[4] = 0;
    stack_ptr[5] = 0;
    stack_ptr[6] = 0;
    stack_ptr[7] = 0;

    // Need to store rest of registers that isn't included in the exception frame onto stack.
    // R4-R11 and LR
    stack_ptr -= 9;
    for (int i = 0; i < 9; i++)
    {
        stack_ptr[i] = 0;
    }

    task1_tcb->stck_ptr = stack_ptr;

    NVIC_SetPriority(-2, 15);
    highest_prio_task_ptr = task1_tcb;
    __set_PSP(stack_ptr);
    __set_CONTROL(0x02);         // CONTROL.SPSEL = 1 → use PSP in Thread mode
    __ISB();                     // Instruction sync barrier

    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void task1()
{
    while(1) {};
}

// void task2()
// {

// }

void exit()
{
    bool doNothing = true;
    while(doNothing) {};
}
