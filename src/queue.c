#include "thinkernel_priv.h"

struct Queue
{
    void* queue;
    uint32_t size;     // Size in bytes of one slot
    uint32_t capacity; // Number of slots in the queue

    uint32_t next_free_slot; // 0-based
};

bool create_queue(uint32_t queue_length, uint32_t queue_data_size_bytes, Queue_t* queue_rtn)
{
    if ( queue_length == 0 || queue_data_size_bytes == 0 || queue_rtn == NULL )
        return false;

    // Allocate space for the queue

    // Initialize queue members

    return true;
}

bool push_queue(Queue_t* queue, void* data, int32_t ms)
{
    if ( queue == NULL || data == NULL )
        return false;

    // disable ctx switching

    // If queue full
    if ( queue->next_free_slot == ( queue->capacity - 1 ) )
    {
        // enable ctx switching

        // don't block on full queue
        if ( ms == 0 )
        {
            return false;
        }
        else if ( ms < 0 )
        {
            // block queue until there is an empty slot
        }
        else // ms > 0
        {
            // block queue for time of ms or
            // if an empty slot becomes available
        }

        // disable ctx switching
    }

    // If queue full after waiting
    if ( queue->next_free_slot == ( queue->capacity - 1 ) )
        return false;

    // Push data on to queue
    // Signal if queue is no longer empty for blocked recievers

    // enable ctx switching

    return true;
}

bool pop_queue(Queue_t* queue, void* data, int32_t ms)
{

}
