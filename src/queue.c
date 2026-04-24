#include "thinkernel_priv.h"

#include <string.h>

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

    disable_ctx_sw();

    // If queue full
    if ( queue->next_free_slot == ( queue->capacity - 1 ) )
    {
        // don't block on full queue so return
        if ( ms == 0 )
        {
            enable_ctx_sw();
            return false;
        }

        // Add task id to waiting_to_send_bitmap
        queue->waiting_to_send_bitmap |= (1U << curr_task_ptr->task_id);

        enable_ctx_sw();

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

        disable_ctx_sw();
    }

    // If queue full after waiting
    if ( queue->next_free_slot == ( queue->capacity - 1 ) )
        return false;

    // Push data on to queue
    void* dest = queue + ( queue->next_free_slot * queue->size );
    memcpy( dest, data, queue->size );

    // Signal if queue is no longer empty for blocked recievers
    if ( queue->waiting_to_receive_bitmap != 0 )
    {
        uint8_t task_id = get_highest_prio_task_idx( queue->waiting_to_receive_bitmap );
        Task_t* task = task_list[task_id];

        // TODO: Logic to add task to ready list can be extracted into its own function
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
        task->task_state = TASK_STATE_READY;

        // WIP: If when task was in delay_list we updated prev along with next
        // removal of task from this delay list would be easier.
        // if ( task->delay > 0U && delay_list != NULL )
        // {
        //     Task_t* delayed_task = delay_list;
        //     while ( delayed_task != task && delayed_task != NULL )
        //     {
        //         delayed_task = delayed_task->next;
        //     }

        //     if ( delayed_task == NULL )
        //     {

        //     }
        // }

        // task->delay = 0U;
    }

    enable_ctx_sw();

    return true;
}

bool receive_queue(Queue_t* queue, void* data, int32_t ms)
{

}
