#include "thinkernel_priv.h"

#include <string.h>

struct Queue
{
    void* queue;
    uint32_t size;     // Size in bytes of one slot
    uint32_t capacity; // Number of slots in the queue

    uint32_t head;
    uint32_t tail;
    uint32_t count;

    Task_t* waiting_to_send_list;
    Task_t* waiting_to_receive_list;
};

bool create_queue(uint32_t queue_length, uint32_t queue_data_size_bytes, Queue_t* queue_rtn)
{
    if ( queue_length == 0 || queue_data_size_bytes == 0 || queue_rtn == NULL )
        return false;

    disable_ctx_sw();

    // Allocate space for the queue TODO: Replace with own allocation implementation
    queue_rtn->queue = malloc( queue_data_size_bytes * queue_length );
    if ( queue_rtn->queue == NULL )
    {
        enable_ctx_sw();
        return false;
    }

    // Initialize queue members
    queue_rtn->size = queue_data_size_bytes;
    queue_rtn->capacity = queue_length;

    queue_rtn->head = 0U;
    queue_rtn->tail = 0U;
    queue_rtn->count = 0U;
    queue_rtn->waiting_to_send_list = NULL;
    queue_rtn->waiting_to_receive_list = NULL;

    enable_ctx_sw();

    return true;
}

static bool handle_queue_full( Queue_t* queue, int32_t ms )
{
    // Don't block on 0 ms delay
    if ( ms == 0 )
        return false;

    if ( !change_task_state( curr_task_ptr, TASK_STATE_WAITING_SEND, &queue->waiting_to_send_list ) )
        return false;

    if ( ms > 0 )
    {
        if ( !set_timer( ms, false ) )
            return false;
    }
    else // ms < 0
    {
        // Block indefinitely until a slot becomes available
    }

    enable_ctx_sw();
    schedule();
    disable_ctx_sw();

    // If queue still full then return
    if ( queue->count == queue->capacity )
        return false;

    return true;
}

static bool handle_queue_new_data( Queue_t* queue )
{
    Task_t* task = queue->waiting_to_receive_list;
    if ( !change_task_state( task, TASK_STATE_READY, &queue->waiting_to_receive_list ) )
        return false;

    // Attempt to clear timer, ignore result
    clear_timer( task );

    enable_ctx_sw();
    schedule();
    disable_ctx_sw();

    return true;
}

static bool handle_queue_empty( Queue_t* queue, int32_t ms )
{
    // Don't block on 0 ms delay
    if ( ms == 0 )
        return false;

    if ( !change_task_state( curr_task_ptr, TASK_STATE_WAITING_RECV, &queue->waiting_to_receive_list ) )
        return false;

    if ( ms > 0 )
    {
        if ( !set_timer( ms, false ) )
            return false;
    }
    else // ms < 0
    {
        // Block indefinitely until a slot becomes available
    }

    enable_ctx_sw();
    schedule();
    disable_ctx_sw();

    // If queue still full then return
    if ( queue->count == 0U )
        return false;

    return true;
}

static bool handle_queue_new_slot( Queue_t* queue )
{
    Task_t* task = queue->waiting_to_send_list;
    if ( !change_task_state( task, TASK_STATE_READY, &queue->waiting_to_send_list ) )
        return false;

    // Attempt to clear timer, ignore result
    clear_timer( task );

    enable_ctx_sw();
    schedule();
    disable_ctx_sw();

    return true;
}

bool send_queue(Queue_t* queue, void* data, int32_t ms)
{
    if ( queue == NULL || data == NULL )
        return false;

    disable_ctx_sw();

    if ( queue->count == queue->capacity )
    {
        if ( !handle_queue_full( queue, ms ) )
        {
            enable_ctx_sw();
            return false;
        }
    }

    // Push data on to queue
    void* dest = (uint8_t*)queue->queue + ( queue->tail * queue->size );
    memcpy( dest, data, queue->size );
    queue->tail = ( queue->tail + 1 ) % queue->capacity;
    queue->count++;

    // If there are blocked recievers waiting on data
    if ( queue->waiting_to_receive_list != NULL )
    {
        if ( !handle_queue_new_data( queue ) )
        {
            enable_ctx_sw();
            return false;
        }
    }

    enable_ctx_sw();

    return true;
}

bool receive_queue(Queue_t* queue, void* data, int32_t ms)
{
    if ( queue == NULL || data == NULL )
        return false;

    disable_ctx_sw();

    if ( queue->count == 0U )
    {
        if ( !handle_queue_empty( queue, ms ) )
        {
            enable_ctx_sw();
            return false;
        }
    }

    void* src = (uint8_t*)queue->queue + ( queue->head * queue->size );
    memcpy( data, src, queue->size );
    queue->head = ( queue->head + 1 ) % queue->capacity;
    queue->count--;

    if ( queue->waiting_to_send_list != NULL )
    {
        if ( !handle_queue_new_slot( queue ) )
        {
            enable_ctx_sw();
            return false;
        }
    }

    enable_ctx_sw();

    return true;
}
