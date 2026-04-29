#include "thinkernel_priv.h"

#define TASK_IDLE_SIZE ( 0x48U )

uint32_t idle_stack[TASK_IDLE_SIZE];
Task_t idle_task;
Task_t* idle_task_ptr = &idle_task;

// TODO: Implement low power mode
void idle_loop()
{
    bool doNothing = true;
    while(doNothing) {};
}

void schedule()
{
    // if no task is ready to run switch to the idle task
    // else get and set the highest priority ready to run task
    if ( ready_bitmap == 0 )
    {
        highest_prio_task_ptr = idle_task_ptr;
    }
    else
    {
        uint8_t idx = get_highest_bitmap_idx(ready_bitmap);
        highest_prio_task_ptr = ready_list[idx];
    }

    // If task state did not change to running due to error or curr_task_ptr
    // is still the highest_prio_task_ptr then no need to reschedule it.
    if ( change_task_state( highest_prio_task_ptr, TASK_STATE_RUNNING, NULL ) )
        return;

    if ( curr_task_ptr != highest_prio_task_ptr )
    {
        context_switch();
    }
}

void start_thinkernel()
{
    init_platform();

    // TODO: Wrap the creation of a task from the kernel into a kernel call
    // that is not available to the API
    init_stack(idle_task_ptr, idle_stack, TASK_IDLE_SIZE, idle_loop);
    idle_task_ptr->task_id = 0;
    idle_task_ptr->priority = 0;
    idle_task_ptr->task_state = TASK_STATE_READY;
    idle_task_ptr->next = NULL;
    idle_task_ptr->prev = NULL;

    schedule();
}
