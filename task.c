#include "task.h"

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
        if (ready_list[new_task->priority] == NULL)
        {
            new_task->prev = NULL;
            new_task->next = NULL;
            ready_list[new_task->priority] = new_task;
        }
        else
        {
            new_task->prev = ready_list[new_task->priority]->prev;
            ready_list[new_task->priority]->prev->next = new_task;
            new_task->next = ready_list[new_task->priority];
        }
        ready_bitmap |= (1 << new_task->priority);

        if ( curr_task_ptr == NULL )
        {
            curr_task_ptr = new_task;
            // perform context switch trigger pendsv handler
        }
        else if ( new_task->priority > curr_task_ptr->priority )
        {
            // perform context switch trigger pendsv handler
        }

        // // First task created
        // if ( curr_task_ptr == NULL )
        // {
        //     curr_task_ptr = new_task;
        //     new_task->task_state = TASK_STATE_RUNNING;
        //     new_task->entry_point();
        // }
        // else
        // {
        //     new_task->task_state = TASK_STATE_READY;
        //     ready_list[task_id] = new_task;
        // }
    }
    else
    {
        // new_task->task_state = TASK_STATE_BLOCKED;
        // blocked_list[task_id] = new_task;
    }

    return true;
}
