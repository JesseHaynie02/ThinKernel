#include "thinkernel_priv.h"

#include "stm32f303x8.h" // TODO: Relocate platform specific calls

#define HSI_VALUE           ( (uint32_t)8000000U )  // Value of the Internal oscillator in Hz
#define THOUSAND_HERTZ      ( (uint32_t)1000U )
#define SYS_TICK_RELOAD     ( (HSI_VALUE / THOUSAND_HERTZ) - 1 )

uint32_t systick_ctr = 0;

void init_systick()
{
    SysTick->LOAD = SYS_TICK_RELOAD;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                    | SysTick_CTRL_TICKINT_Msk
                    | SysTick_CTRL_ENABLE_Msk;
}

// TODO: If case should be handled in a kernel task triggered by a semaphore in this interrupt instead of handling it here

void systick_handler()
{
    systick_ctr++;

    if (delay_list == NULL)
    {
        return;
    }

    if (--delay_list->delay == 0)
    {
        Task_t* task = delay_list;
        delay_list = delay_list->next;
        task->next = NULL;
        task->task_state = TASK_STATE_READY;

        Task_t* head = ready_list[task->priority];
        if (head == NULL)
        {
            task->prev = task;
            task->next = task;
            ready_list[task->priority] = task;
        }
        else
        {
            Task_t* tail = head->prev;

            task->next = head;
            task->prev = tail;
            tail->next = task;
            head->prev = task;
        }
        ready_bitmap |= (1 << task->priority);

        // Switch to highest priority ready to run task if it changed
        schedule();

        if (curr_task_ptr != highest_prio_task_ptr)
        {
            context_switch();
        }
    }

    return;
}
