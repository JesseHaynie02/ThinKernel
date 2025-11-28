#include "thinkernel_priv.h"

uint32_t sem_bitmap = 0;
Sem_t* sem_list[MAX_NUM_SEMS];

// TODO: Need to make thread safe
bool create_semaphore(uint32_t sem_id, Sem_t* semaphore, uint32_t init_val)
{
    if ( sem_id >= MAX_NUM_SEMS )
        return false;
    // If semaphore with sem id already exists
    if ( sem_bitmap & (1U << sem_id) || sem_list[sem_id] != NULL )
        return false;
    if ( semaphore == NULL )
        return false;

    // Register new semaphore
    sem_bitmap |= (1U << sem_id);
    sem_list[sem_id] = semaphore;

    semaphore->sem_id = sem_id;
    semaphore->sem_val = init_val;
    semaphore->wait_list = NULL;

    return true;
}
