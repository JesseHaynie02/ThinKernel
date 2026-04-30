#include "thinkernel_priv.h"

#define MAX_NUM_TIMERS 32

typedef struct Timer Timer_t;

struct Timer
{
    uint8_t timer_id;
    uint32_t delay;
    Task_t* task;
    struct Timer* next;
    struct Timer* prev;
};

uint32_t timer_bitmap = 0xFFFFU;
Timer_t timer_list[MAX_NUM_TIMERS];
Timer_t* delay_list = NULL;

static Timer_t* get_timer_instance( uint8_t* timer_id )
{
    // If all timers are being used return null
    if ( timer_bitmap == 0U )
        return NULL;

    uint8_t next_available_timer = get_highest_bitmap_idx( timer_bitmap );

    // Clear this timer from the timer bitmap since it is now being used
    timer_bitmap &= ~(1U << next_available_timer);

    // Set timer_id and return timer instance
    *timer_id = next_available_timer;
    return &timer_list[next_available_timer];
}

static void return_timer_instance( uint8_t timer_id )
{
    // Add timer back to the timer resource pool
    timer_bitmap |= ( 1U << timer_id );
}

// TODO: Need to make thread safe.
bool delay_task( uint32_t ms )
{
    if ( ms == 0 )
        return false;
    if ( curr_task_ptr->task_state != TASK_STATE_RUNNING )
        return false;
    if ( curr_task_ptr != ready_list[curr_task_ptr->priority] )
        return false;

    uint8_t timer_id;
    Timer_t* timer = get_timer_instance( &timer_id );
    if ( timer == NULL )
        return false;

    timer->timer_id = timer_id;
    timer->delay = ms;
    timer->task = curr_task_ptr;

    Timer_t* next_timer = delay_list;
    Timer_t* prev_timer = NULL;

    // Find where to insert this timer into the delay list
    // by calculating the delay delta and comparing the offset
    while (next_timer != NULL && timer->delay >= next_timer->delay)
    {
        timer->delay -= next_timer->delay;
        prev_timer = next_timer;
        next_timer = next_timer->next;
    }

    // Insert timer into delay list
    timer->next = next_timer;
    timer->prev = prev_timer;

    if (next_timer)
    {
        next_timer->delay -= timer->delay;
        next_timer->prev = timer;
    }

    if (prev_timer)
    {
        prev_timer->next = timer;
    }
    else
    {
        delay_list = timer;
    }

    if ( !change_task_state( timer->task, TASK_STATE_DELAY, NULL ) )
        return false;

    // Switch to highest priority ready to run task if it changed
    schedule();

    return true;
}

void service_timers()
{
    if ( delay_list == NULL )
    {
        return;
    }

    if ( --delay_list->delay == 0 )
    {
        // Delay expired, remove timer from head of delay list.
        Timer_t* timer = delay_list;
        delay_list = delay_list->next;
        if ( delay_list != NULL )
        {
            delay_list->prev = NULL;
        }
        timer->next = NULL;
        timer->prev = NULL;

        change_task_state( timer->task, TASK_STATE_READY, NULL );
        return_timer_instance( timer->timer_id );

        // Switch to highest priority ready to run task if it changed
        schedule();
    }
}
