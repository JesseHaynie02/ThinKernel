#include "thinkernel.h"

Task_t* curr_task_ptr = NULL;
Task_t* highest_prio_task_ptr = NULL;

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
    if ( task_id >= MAX_NUM_TASKS || priority >= MAX_PRIORITY)
        return false;
    // If task with task id already exists
    if ( task_bitmap & (1 << task_id) )
        return false;
    if ( entry_point == NULL )
        return false;
    // If stack_addr or stack_size is not 32-bit aligned
    if (((uint32_t)stack_addr & 0x3U) != 0 || (stack_size & 0x3U) != 0)
        return false;

    // Register new task
    task_bitmap |= (1 << task_id);

    init_stack(task, stack_addr, stack_size, entry_point);

    task->task_id = task_id;
    task->priority = priority;
    task->task_state = TASK_STATE_READY;
    task->next = NULL;
    task->prev = NULL;

    // Insert into ready list update ready_bitmap
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
