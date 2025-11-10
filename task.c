#include "task.h"

bool create_task( uint32_t task_id, uint32_t priority, bool startit, void (*entry_point)() )
{
    if ( task_id >= MAX_NUM_TASKS || priority >= MAX_PRIORITY) 
        return false;
    // If task with task id already exists
    if ( task_list & (1 << task_id) )
        return false;
    if ( entry_point == NULL )
        return false;

    // Register new_task
    // TODO: operation needs to be atomic
    task_list |= (1 << task_id);

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
        // First task created
        if ( running == NULL )
        {
            running = new_task;
            new_task->task_state = TASK_STATE_RUNNING;
            new_task->entry_point();
        }
        else
        {
            new_task->task_state = TASK_STATE_READY;
            ready_list[task_id] = new_task;
        }
    }
    else
    {
        new_task->task_state = TASK_STATE_BLOCKED;
        blocked_list[task_id] = new_task;
    }

    return true;
}
