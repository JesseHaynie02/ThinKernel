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
        // If new task is a higher priority than current task
        if ( running->priority < new_task->priority )
        {
            // Move running task into ready list and new task to running
            // Update task state and perform context switch into entry_point
        }
        else
        {
            // Move new task to ready list and update task state
        }
    }
}
