#include "thinkernel.h"

bool create_task( uint32_t task_id, uint32_t priority, bool startit, void (*entry_point)() )
{
    if ( task_id >= MAX_NUM_TASKS || priority >= MAX_PRIORITY) 
        return false;
    // If task with task id already exists
    if ( task_bitmap & (1 << task_id) )
        return false;
    if ( entry_point == NULL )
        return false;

    // Register new_task
    // TODO: operation needs to be atomic
    task_bitmap |= (1 << task_id);

    // TODO: malloc needs to be atomic
    Task_t* new_task = (Task_t*)malloc(sizeof(Task_t));
    if (new_task == NULL)
        return false;

    new_task->task_id = task_id;
    new_task->priority = priority;
    new_task->task_state = TASK_STATE_BLOCKED;
    new_task->entry_point = entry_point;

    if ( startit )
    {
        // Insert into ready list update ready_bitmap
        Task_t* head = ready_list[new_task->priority];
        if (head == NULL)
        {
            new_task->prev = new_task;
            new_task->next = new_task;
            ready_list[new_task->priority] = new_task;
        }
        else
        {
            Task_t* tail = head->prev;

            new_task->next = head;
            new_task->prev = tail;
            tail->next = new_task;
            head->prev = new_task;
        }
        ready_bitmap |= (1 << new_task->priority);

        // Perform context switch
    }
    else
    {
        // Insert into blocked list
    }

    return true;
}

#include "core_cm4.h"

void context_switch()
{
    if (!ready_bitmap)
        return;

    uint32_t idx = 31 - __CLZ(ready_bitmap);
    highest_prio_task_ptr = ready_list[idx];

    // Trigger pendsv interrupt
    
}
