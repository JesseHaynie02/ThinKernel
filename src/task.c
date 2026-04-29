#include "thinkernel_priv.h"

uint32_t task_bitmap = 0U;
uint32_t ready_bitmap = 0U;

Task_t* curr_task_ptr = NULL;
Task_t* highest_prio_task_ptr = NULL;
Task_t* ready_list[MAX_NUM_TASKS];
Task_t* task_list[MAX_NUM_TASKS];

// TODO: Implement proper handling of a task that ended up here
void exit_task()
{
    while(1) {};
}

// TODO: Need to make thread safe or disable the ability to create tasks after the call to start_thinkernel().
bool create_task( uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() )
{
    if ( task_id >= MAX_NUM_TASKS || priority >= MAX_PRIORITY )
        return false;
    // If task with task id already exists
    if ( task_bitmap & ( 1U << task_id ) || task_list[task_id] != NULL )
        return false;
    if ( task == NULL || entry_point == NULL )
        return false;
    // If stack_addr or stack_size is not 32-bit aligned
    if ( ( (uint32_t)stack_addr & 0x3U ) != 0 || (stack_size & 0x3U) != 0 )
        return false;

    // Register new task
    task_bitmap |= ( 1U << task_id );
    task_list[task_id] = task;

    init_stack(task, stack_addr, stack_size, entry_point);

    task->task_id = task_id;
    task->priority = priority;
    task->task_state = TASK_STATE_INITIALIZE;
    task->next = NULL;
    task->prev = NULL;

    if ( !change_task_state( task, TASK_STATE_READY, NULL ) )
        return false;

    return true;
}

// TODO: Need to make thread safe.
bool yield_task(uint32_t task_id)
{
    if ( task_id >= MAX_NUM_TASKS )
        return false;

    Task_t* task = task_list[task_id];

    // If task is not registered in bitmap or task is null
    if ( ( task_bitmap & ( 1 << task_id ) ) == 0 || task == NULL )
        return false;
    // If there are no tasks at the tasks priority or if the head is null
    if ( ( ready_bitmap & ( 1 << task->priority ) ) == 0 || ready_list[task->priority] == NULL )
        return false;
    // If task is blocked or terminated
    if ( task->task_state != TASK_STATE_READY && task->task_state != TASK_STATE_RUNNING )
        return false;

    // Move task to the end of its priorities linked list
    Task_t* head = ready_list[task->priority];
    Task_t* tail = head->prev;

    if ( task == tail )
        return true;

    if ( task == head )
    {
        ready_list[task->priority] = head->next;
    }
    else
    {
        task->next->prev = task->prev;
        task->prev->next = task->next;

        task->next = head;
        task->prev = tail;
        tail->next = task;
        head->prev = task;
    }

    ready_list[task->priority] = task->next;
    curr_task_ptr->task_state = TASK_STATE_READY;

    // Switch to highest priority ready to run task if it changed
    schedule();

    return true;
}
