#include "thinkernel.h"

#define TASK_IDLE_SIZE ( 0x48U )

uint32_t task1_stack[TASK_IDLE_SIZE];
Task_t idle_task;
Task_t* idle_task_ptr = &idle_task;

void delay(volatile uint32_t count)
{
    while (count--) __asm__("nop");
}

// TODO: Implement low power mode
void idle_loop()
{
    bool doNothing = true;
    while(doNothing) {};
}

void context_switch()
{
    // TODO: Implement idle task
    // if (!ready_bitmap)
        // highest_prio_task_ptr = idle_task;

    // else
    uint32_t idx = 31 - __CLZ(ready_bitmap);
    highest_prio_task_ptr = ready_list[idx];

    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
    while(1) {};
}

void start_thinkernel()
{
    __set_PSP(0U);

    // Set the PendSV interrupt as the lowest priority
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    init_systick();

    // TODO: Need to create idle task for when there is nothing to run
    // Idle task doesn't need to be in any task list. When the fetch
    // to get next ready to run task returns nothing then it should
    // return the idle task.
    init_stack(idle_task_ptr, task1_stack, TASK_IDLE_SIZE, idle_loop);

    context_switch();
}
