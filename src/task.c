#include "thinkernel_priv.h"

// TODO: Relocate platform specific functionality

uint32_t task_bitmap = 0U;
uint32_t ready_bitmap = 0U;

Task_t* curr_task_ptr = NULL;
Task_t* highest_prio_task_ptr = NULL;
Task_t* ready_list[MAX_NUM_TASKS];
Task_t* task_list[MAX_NUM_TASKS];
Task_t* delay_list = NULL;

// TODO: Implement proper handling of a task that ended up here
void exit_task()
{
    while(1) {};
}

void init_stack( Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() )
{
    // Descening stack so location starts at the highest address
    uint32_t* task_stack_loc = stack_addr + stack_size;

    // Make sure the stack location is 32 bit aligned
    task_stack_loc = (uint32_t *)((uint32_t)task_stack_loc & 0xFFFFFFF8U);

    /************************************ Exception Frame ************************************/
    // Need to store exception frame onto stack for a new task.
    *(--task_stack_loc) = 0x01000000U;              // xPSR, execute instructions in thumb state
    *(--task_stack_loc) = (uint32_t)entry_point;    // PC, entry point for this task
    *(--task_stack_loc) = (uint32_t)exit_task;      // LR, exit point for this task

    // Prefill R0, R1, R2, R3, R12
    *(--task_stack_loc) = 0x12121212U;
    *(--task_stack_loc) = 0x03030303U;
    *(--task_stack_loc) = 0x02020202U;
    *(--task_stack_loc) = 0x01010101U;
    *(--task_stack_loc) = 0x00000000U;
    /************************************ Exception Frame ************************************/

    // Store the rest of the registers that aren't in the exception frame onto the stack
    // R4, R5, R6, R7, R8, R9, R10, R11, LR
    *(--task_stack_loc) = 0xFFFFFFFDUL;     // LR, tells the exception handler how to return

    // Prefill R4, R5, R6, R7, R8, R9, R10, R11
    *(--task_stack_loc) = 0x11111111U;
    *(--task_stack_loc) = 0x10101010U;
    *(--task_stack_loc) = 0x09090909U;
    *(--task_stack_loc) = 0x08080808U;
    *(--task_stack_loc) = 0x07070707U;
    *(--task_stack_loc) = 0x06060606U;
    *(--task_stack_loc) = 0x05050505U;
    *(--task_stack_loc) = 0x04040404U;

    task->stack_ptr = task_stack_loc;
    task->stack_limit = stack_addr;
}

bool create_task( uint32_t task_id, uint32_t priority, Task_t* task, uint32_t* stack_addr, uint32_t stack_size, void (*entry_point)() )
{
    if ( task_id >= MAX_NUM_TASKS || priority >= MAX_PRIORITY )
        return false;
    // If task with task id already exists
    if ( task_bitmap & (1U << task_id) || task_list[task_id] != NULL )
        return false;
    if ( entry_point == NULL )
        return false;
    // If stack_addr or stack_size is not 32-bit aligned
    if ( ((uint32_t)stack_addr & 0x3U) != 0 || (stack_size & 0x3U) != 0 )
        return false;

    // Register new task
    task_bitmap |= (1U << task_id);
    task_list[task_id] = task;

    init_stack(task, stack_addr, stack_size, entry_point);

    task->task_id = task_id;
    task->priority = priority;
    task->task_state = TASK_STATE_READY;
    task->delay = 0;
    task->next = NULL;
    task->prev = NULL;

    // Insert into ready list update ready bitmap
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

    return true;
}

bool yield_task(uint32_t task_id)
{
    if (task_id >= MAX_NUM_TASKS)
        return false;

    Task_t* task = task_list[task_id];

    // If task is not registered in bitmap or task is null
    if ( (task_bitmap & (1 << task_id)) == 0 || task == NULL )
        return false;
    // If there are no tasks at the tasks priority or if the head is null
    if ( (ready_bitmap & (1 << task->priority)) == 0 || ready_list[task->priority] == NULL )
        return false;
    // If task is blocked or terminated
    if ( task->task_state != TASK_STATE_READY && task->task_state != TASK_STATE_RUNNING )
        return false;

    // Move task to the end of its priorities linked list
    Task_t* head = ready_list[task->priority];
    Task_t* tail = head->prev;

    // Nothing to do
    if (task == tail)
        return true;

    if (task == head)
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

    // Switch to highest priority ready to run task if it changed
    schedule();

    if (curr_task_ptr != highest_prio_task_ptr)
    {
        context_switch();
    }

    return true;
}

// TODO: Do we need to disable context switching?

bool delay_task(uint32_t ms)
{
    if (ms == 0)
        return false;
    // TODO: State should be running only but need to figure out how to update running correctly
    if (curr_task_ptr->task_state != TASK_STATE_READY)
        return false;
    if (curr_task_ptr != ready_list[curr_task_ptr->priority])
        return false;

    curr_task_ptr->delay = ms;
    curr_task_ptr->task_state = TASK_STATE_BLOCKED;

    if (curr_task_ptr != curr_task_ptr->next && curr_task_ptr != curr_task_ptr->prev)
    {
        ready_list[curr_task_ptr->priority] = curr_task_ptr->next;
        curr_task_ptr->prev->next = curr_task_ptr->next;
        curr_task_ptr->next->prev = curr_task_ptr->prev;
    }
    else
    {
        ready_list[curr_task_ptr->priority] = NULL;
        ready_bitmap &= ~(1 << curr_task_ptr->priority);
    }

    curr_task_ptr->next = NULL;
    curr_task_ptr->prev = NULL;

    Task_t* task = delay_list;
    Task_t* prev = NULL;

    while (task != NULL && curr_task_ptr->delay >= task->delay)
    {
        curr_task_ptr->delay -= task->delay;
        prev = task;
        task = task->next;
    }

    curr_task_ptr->next = task;

    if (task)
    {
        task->delay -= curr_task_ptr->delay;
    }

    if (prev)
    {
        prev->next = curr_task_ptr;
    }
    else
    {
        delay_list = curr_task_ptr;
    }

    // Switch to highest priority ready to run task if it changed
    schedule();

    if (curr_task_ptr != highest_prio_task_ptr)
    {
        context_switch();
    }

    return true;
}
