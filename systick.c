#include "thinkernel.h"

#define HSI_VALUE           ( (uint32_t)8000000U )  // Value of the Internal oscillator in Hz
#define ONE_HERTZ           ( (uint32_t)1U )
#define SYS_TICK_RELOAD     ( (HSI_VALUE / ONE_HERTZ) - 1 )

uint32_t systick_ctr = 0;

void init_systick()
{
    SysTick->LOAD = SYS_TICK_RELOAD;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                    | SysTick_CTRL_TICKINT_Msk
                    | SysTick_CTRL_ENABLE_Msk;
}

void systick_handler()
{
    systick_ctr++;
}
