#include "thinkernel_priv.h"

// TODO: platform shouldn't have references to specific device
#include "stm32f303x8.h"

#define BASEPRI_MASK_PENDSV_SYSTICK   14U
#define BASEPRI_UNMASK_ALL            0U

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

// TODO: Add comments
// TODO: resolve dependency on specific device
void init_platform()
{
    __set_PSP(0U);

    NVIC_SetPriority(MemoryManagement_IRQn, (1 << __NVIC_PRIO_BITS) - 6);
    NVIC_SetPriority(BusFault_IRQn, (1 << __NVIC_PRIO_BITS) - 5);
    NVIC_SetPriority(UsageFault_IRQn, (1 << __NVIC_PRIO_BITS) - 4);
    NVIC_SetPriority(SVCall_IRQn, (1 << __NVIC_PRIO_BITS) - 3);
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    init_systick();
}

void enable_ctx_sw()
{
    __set_BASEPRI(BASEPRI_MASK_PENDSV_SYSTICK);
}

void disable_ctx_sw()
{
    __set_BASEPRI(BASEPRI_UNMASK_ALL);
}

void context_switch()
{
    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
}

uint8_t get_highest_prio_task_idx(uint32_t task_bitmap)
{
    return (31 - __CLZ(task_bitmap));
}

void systick_handler()
{
    systick_ctr++;

    if (delay_list == NULL)
    {
        return;
    }

    // TODO: If case should be handled in a kernel task triggered by a semaphore in this interrupt instead of handling it here
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
