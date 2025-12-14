#include "thinkernel_priv.h"

#include "stm32f303x8.h" // TODO: Relocate platform specific calls

#define SEM_VAL_MAX ( 4294967295U )

uint32_t sem_bitmap = 0;
Sem_t* sem_list[MAX_NUM_SEMS];

bool create_semaphore(uint32_t sem_id, Sem_t* semaphore, uint32_t init_val)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;
    if ( semaphore == NULL )
        return false;

    __set_BASEPRI(BASEPRI_MASK_PENDSV_SYSTICK);

    // If semaphore with sem id already exists
    if ( sem_bitmap & (1U << sem_id) || sem_list[sem_id] != NULL )
    {
        __set_BASEPRI(BASEPRI_UNMASK_ALL);
        return false;
    }

    semaphore->sem_id = sem_id;
    semaphore->sem_val = init_val;
    semaphore->wait_list = NULL;

    // Register new semaphore
    sem_bitmap |= (1U << sem_id);
    sem_list[sem_id] = semaphore;

    __set_BASEPRI(BASEPRI_UNMASK_ALL);

    return true;
}

bool post_semaphore(uint32_t sem_id)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;

    __set_BASEPRI(BASEPRI_MASK_PENDSV_SYSTICK);

    // If semaphore with sem id doesn't exist or semaphore null
    if ( (sem_bitmap & (1U << sem_id)) == 0 || sem_list[sem_id] == NULL )
    {
        __set_BASEPRI(BASEPRI_UNMASK_ALL);
        return false;
    }

    Sem_t* semaphore = sem_list[sem_id];

    if (semaphore->wait_list == NULL)
    {
        if (semaphore->sem_val <= SEM_VAL_MAX - 1U)
        {
            semaphore->sem_val++;
        }
    }
    else
    {
        Task_t* task = semaphore->wait_list;
        semaphore->wait_list = semaphore->wait_list->next;
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
    }

    __set_BASEPRI(BASEPRI_UNMASK_ALL);

    // Switch to highest priority ready to run task if it changed
    schedule();

    if (curr_task_ptr != highest_prio_task_ptr)
    {
        context_switch();
    }

    return true;
}

bool wait_for_semaphore(uint32_t sem_id)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;

    __set_BASEPRI(BASEPRI_MASK_PENDSV_SYSTICK);

    if ( (sem_bitmap & (1U << sem_id)) == 0 || sem_list[sem_id] == NULL )
    {
        __set_BASEPRI(BASEPRI_UNMASK_ALL);
        return false;
    }

    Sem_t* semaphore = sem_list[sem_id];

    if (semaphore->sem_val > 0 && semaphore->wait_list == NULL)
    {
        semaphore->sem_val--;
    }
    else
    {
        Task_t* running_task = curr_task_ptr;
        Task_t* prev_task = sem_list[sem_id]->wait_list;
        Task_t* curr_task = sem_list[sem_id]->wait_list;
        while (curr_task != NULL && curr_task->priority >= running_task->priority)
        {
            prev_task = curr_task;
            curr_task = curr_task->next;
        }

        if (ready_list[running_task->priority] == curr_task_ptr)
        {
            if (ready_list[running_task->priority] == ready_list[running_task->priority]->next &&
                ready_list[running_task->priority] == ready_list[running_task->priority]->prev)
            {
                ready_list[running_task->priority]->next = NULL;
                ready_list[running_task->priority]->prev = NULL;
                ready_list[running_task->priority] = NULL;
                ready_bitmap &= ~(1 << running_task->priority);
            }
            else
            {
                ready_list[running_task->priority]->prev->next = ready_list[running_task->priority]->next;
                ready_list[running_task->priority]->next->prev = ready_list[running_task->priority]->prev;
                ready_list[running_task->priority] = ready_list[running_task->priority]->next;
            }
        }
        else
        {
            __set_BASEPRI(BASEPRI_UNMASK_ALL);
            return false;
        }

        running_task->task_state = TASK_STATE_BLOCKED;
        running_task->next = curr_task;
        running_task->prev = NULL;

        if (semaphore->wait_list != NULL)
        {
            prev_task->next = running_task;
        }
        else
        {
            semaphore->wait_list = running_task;
        }

        __set_BASEPRI(BASEPRI_UNMASK_ALL);

        // Switch to highest priority ready to run task if it changed
        schedule();

        if (curr_task_ptr != highest_prio_task_ptr)
        {
            context_switch();
        }
    }

    return true;
}
