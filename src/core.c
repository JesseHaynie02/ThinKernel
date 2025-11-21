#include "thinkernel.h"

#define TASK_IDLE_SIZE ( 0x48U )

uint32_t idle_stack[TASK_IDLE_SIZE];
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
    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
}

void schedule()
{
    // if no task is ready to run switch to the idle task
    // else get the highest priority ready to run task and run it
    if (!ready_bitmap)
    {
        highest_prio_task_ptr = idle_task_ptr;
    }
    else
    {
        uint32_t idx = 31 - __CLZ(ready_bitmap);
        highest_prio_task_ptr = ready_list[idx];
    }
}

void start_thinkernel()
{
    __set_PSP(0U);

    // Set the PendSV interrupt as the lowest priority
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    init_systick();

    // TODO: Wrap the creation of a task from the kernel into a kernel call
    // that is not available to the API
    init_stack(idle_task_ptr, idle_stack, TASK_IDLE_SIZE, idle_loop);
    idle_task_ptr->task_id = 0;
    idle_task_ptr->priority = 0;
    idle_task_ptr->task_state = TASK_STATE_READY;
    idle_task_ptr->next = NULL;
    idle_task_ptr->prev = NULL;

    schedule();
    context_switch();
}
