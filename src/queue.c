#include "thinkernel_priv.h"

struct Queue
{
    void* queue;
    uint32_t size;     // Size in bytes of one slot
    uint32_t capacity; // Number of slots in the queue

    uint32_t next_free_slot; // 0-based

    uint32_t waiting_to_send_bitmap;
    uint32_t waiting_to_receive_bitmap;
};

bool create_queue(uint32_t queue_length, uint32_t queue_data_size_bytes, Queue_t* queue_rtn)
{
    if ( queue_length == 0 || queue_data_size_bytes == 0 || queue_rtn == NULL )
        return false;

    // Allocate space for the queue

    // Initialize queue members

    return true;
}

bool send_queue(Queue_t* queue, void* data, int32_t ms)
{
    if ( queue == NULL || data == NULL )
        return false;

    // disable ctx switching

    // If queue full
    if ( queue->next_free_slot == ( queue->capacity - 1 ) )
    {
        // don't block on full queue so return
        if ( ms == 0 )
        {
            return false;
        }

        // Add task id to waiting_to_send_bitmap

        // enable ctx switching

        if ( ms > 0 )
        {
            // Delay task until either delay expires
            // or slot becomes available.
            delay_task( ms );
        }
        else // ms < 0
        {
            // Block task until slot becomes available.
            block_task( curr_task_ptr );

            schedule();

            if (curr_task_ptr != highest_prio_task_ptr)
            {
                context_switch();
            }
        }

        // If queue still full then return
        if ( queue->next_free_slot == ( queue->capacity - 1 ) )
            return false;

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

bool receive_queue(Queue_t* queue, void* data, int32_t ms)
{

}
