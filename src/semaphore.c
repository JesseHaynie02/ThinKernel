#include "thinkernel_priv.h"

#include "stm32f303x8.h" // TODO: Relocate platform specific calls

#include <stdint.h>

uint32_t sem_bitmap = 0;
Sem_t* sem_list[MAX_NUM_SEMS];

bool create_semaphore(uint32_t sem_id, Sem_t* semaphore, uint32_t init_val)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;
    if ( semaphore == NULL )
        return false;

    disable_ctx_sw();

    // If semaphore with sem id already exists
    if ( sem_bitmap & (1U << sem_id) || sem_list[sem_id] != NULL )
    {
        enable_ctx_sw();
        return false;
    }

    semaphore->sem_id = sem_id;
    semaphore->sem_val = init_val;
    semaphore->wait_list = NULL;

    // Register new semaphore
    sem_bitmap |= (1U << sem_id);
    sem_list[sem_id] = semaphore;

    enable_ctx_sw();

    return true;
}

bool post_semaphore(uint32_t sem_id)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;

    disable_ctx_sw();

    // If semaphore with sem id doesn't exist or semaphore null
    if ( (sem_bitmap & (1U << sem_id)) == 0 || sem_list[sem_id] == NULL )
    {
        enable_ctx_sw();
        return false;
    }

    Sem_t* semaphore = sem_list[sem_id];

    if ( semaphore->wait_list == NULL )
    {
        // Prevent integer overflow
        if ( semaphore->sem_val < UINT32_MAX )
        {
            semaphore->sem_val++;
        }
    }
    else
    {
        if ( !change_task_state( curr_task_ptr, TASK_STATE_READY,
                                 &semaphore->wait_list ) )
            return false;
    }

    enable_ctx_sw();

    // Switch to highest priority ready to run task if it changed
    schedule();

    return true;
}

bool wait_for_semaphore(uint32_t sem_id)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;

    disable_ctx_sw();

    if ( (sem_bitmap & (1U << sem_id)) == 0 || sem_list[sem_id] == NULL )
    {
        enable_ctx_sw();
        return false;
    }

    Sem_t* semaphore = sem_list[sem_id];

    if (semaphore->sem_val > 0 && semaphore->wait_list == NULL)
    {
        semaphore->sem_val--;
    }
    else
    {
        if ( change_task_state( curr_task_ptr, TASK_STATE_WAITING_SEMA,
                                &semaphore->wait_list ) )
            return false;
    }

    enable_ctx_sw();

    // Switch to highest priority ready to run task if it changed
    schedule();

    return true;
}
