#include "thinkernel_priv.h"

static bool insert_into_ready_list( Task_t* task )
{
    // Ready list is an array where each index represents
    // a priority and each index contains a circular
    // doubly linked list.

    Task_t* head = ready_list[task->priority];
    if ( head == NULL )
    {
        // Insert into empty list
        task->prev = task;
        task->next = task;
        ready_list[task->priority] = task;
    }
    else
    {
        // Insert into non-empty list
        Task_t* tail = head->prev;

        task->next = head;
        task->prev = tail;
        tail->next = task;
        head->prev = task;
    }

    // Signal this priority has atleast one ready to run task
    ready_bitmap |= ( 1 << task->priority );
    return true;
}

static bool remove_from_ready_list( Task_t* task )
{
    if ( task != curr_task_ptr )
        return false;
    if ( task != ready_list[task->priority] )
        return false;
    if ( task->task_state != TASK_STATE_RUNNING )
        return false;

    if ( task != task->next && task != task->prev )
    {
        // Remove from list that more than one task
        ready_list[task->priority] = task->next;
        task->prev->next = task->next;
        task->next->prev = task->prev;
    }
    else
    {
        // Remove from list that only has one task
        ready_list[task->priority] = NULL;

        // Signal this priority has no tasks ready to run
        ready_bitmap &= ~(1 << task->priority);
    }

    task->next = NULL;
    task->prev = NULL;

    return true;
}

static bool insert_into_blocked_list( Task_t** blocked_list, Task_t* task )
{
    if ( blocked_list == NULL )
        return false;

    if ( *blocked_list == NULL )
    {
        // Insert into empty list
        *blocked_list = task;
        task->next = NULL;
        task->prev = NULL;
    }
    else
    {
        // Insert task into non-empty list based on priority
        Task_t* next_task = *blocked_list;
        Task_t* prev_task = NULL;

        while ( next_task != NULL && task->priority <= next_task->priority )
        {
            prev_task = next_task;
            next_task = next_task->next;
        }

        task->next = next_task;
        task->prev = prev_task;

        if ( prev_task != NULL )
            prev_task->next = task;
        else
            *blocked_list = task;

        if ( next_task != NULL )
            next_task->prev = task;
    }

    return true;
}

static bool remove_from_blocked_list( Task_t** blocked_list, Task_t* task )
{
    if ( blocked_list == NULL )
        return false;

    // Remove from blocked list, handle cases where its at the head, tail or middle.
    if ( task->next != NULL )
        task->next->prev = task->prev;

    if ( task->prev != NULL )
        task->prev->next = task->next;
    else
        *blocked_list = task->next;

    task->next = NULL;
    task->prev = NULL;
}

static bool ready_to_running( Task_t* task )
{
    if ( task->task_state != TASK_STATE_READY )
        return false;

    // If the currently running task is still the highest
    // priority ready to run task then return false.
    if ( curr_task_ptr == highest_prio_task_ptr )
        return false;
    // If the new highest priority ready to run task
    // isn't the same as task then something is wrong.
    if ( highest_prio_task_ptr != task )
        return false;

    if ( !change_task_state( curr_task_ptr, TASK_STATE_PREEMPTED, NULL ) )
        return false;

    return true;
}

static bool running_to_preempted( Task_t* task )
{
    if ( task->task_state != TASK_STATE_RUNNING )
        return false;

    return true;
}

static bool running_to_blocked( Task_t** blocked_list, Task_t* task )
{
    if ( task->task_state != TASK_STATE_RUNNING )
        return false;

    if ( !remove_from_ready_list( task ) )
        return false;

    if ( !insert_into_blocked_list( blocked_list, task ) )
        return false;

    return true;
}

static bool running_to_delayed( Task_t* task )
{
    if ( task->task_state != TASK_STATE_RUNNING )
        return false;

    return remove_from_ready_list( task );
}

static bool blocked_to_ready( Task_t** blocked_list, Task_t* task )
{
    if ( task->task_state != TASK_STATE_DELAY &&
         task->task_state != TASK_STATE_WAITING_SEND &&
         task->task_state != TASK_STATE_WAITING_RECV &&
         task->task_state != TASK_STATE_WAITING_SEMA )
    {
        return false;
    }

    if ( !remove_from_blocked_list( blocked_list, task ) )
        return false;

    if ( !insert_into_ready_list( task ) )
        return false;

    return true;
}

bool change_task_state( Task_t* task, TaskState_t change_to_state, Task_t** task_list )
{
    if ( task == NULL )
        return false;

    bool status = false;

    switch ( change_to_state )
    {
        case TASK_STATE_RUNNING:
            status = ready_to_running( task );
            break;
        case TASK_STATE_PREEMPTED:
            status = running_to_preempted( task );
            break;
        case TASK_STATE_DELAY:
            status = running_to_delayed( task );
            break;
        case TASK_STATE_WAITING_SEND:
        case TASK_STATE_WAITING_RECV:
        case TASK_STATE_WAITING_SEMA:
            status = running_to_blocked( task_list, task );
            break;
        case TASK_STATE_READY:
            if ( task->task_state == TASK_STATE_INITIALIZE )
                status = insert_into_ready_list( task );
            else
                status = blocked_to_ready( task_list, task );
            break;
        default:
            status = false;
            break;
    }

    // Attemp to update task state if switch case was successful
    if ( status )
        task->task_state = change_to_state;

    return status;
}
