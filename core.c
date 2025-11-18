#include "thinkernel.h"

void delay(volatile uint32_t count)
{
    while (count--) __asm__("nop");
}

void start_thinkernel()
{
    __set_PSP(0U);

    // Set the PendSV interrupt as the lowest priority
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    init_systick();

    // Need to create idle task for when there is nothing to run
    // Idle task doesn't need to be in any task list. When the fetch
    // to get next ready to run task returns nothing then it should
    // return the idle task.


    // Trigger the PendSV interrupt and wait for the context switch to occur
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    __DSB();
    __ISB();
    while(1) {};
}
