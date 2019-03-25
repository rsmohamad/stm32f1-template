#include "ticks.h"

volatile u16 msTicks = 0;
volatile u16 sTicks = 0;

u16 get_ms_ticks ()
{
    return msTicks;
}

u16 get_seconds ()
{
    return sTicks;
}

void ticks_init ()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd (TICKS_RCC, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 1000;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit (TICKS_TIM, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit (TICKS_TIM, TIM_IT_Update);
    TIM_ITConfig (TICKS_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd (TICKS_TIM, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = TICKS_IRQn;
    NVIC_Init (&NVIC_InitStructure);

}

TICKS_IRQHandler
{
    if (TIM_GetITStatus (TICKS_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit (TICKS_TIM, TIM_IT_Update);

        if (++msTicks >= 1000) {
            msTicks = 0;
            sTicks++;
        }

    }
}

static __IO uint32_t TimingDelay;
u8 using_delay = 0;

/**
  * @brief  Generate a delay (in us)
  * @param  nus: us to be delayed
  * @retval None
  */
void _delay_us (u32 nus)
{
    u32 temp;
    if (using_delay == 0) {
        using_delay = 1;
        SysTick->LOAD = 9 * nus;
        SysTick->VAL = 0x00;
        SysTick->CTRL = 0x01;
        do {
            temp = SysTick->CTRL;
        }
        while ((temp & 0x01) && (!(temp & (1 << 16))));
        SysTick->CTRL = 0x00;
        SysTick->VAL = 0x00;
        using_delay = 0;
    }
    else {
        nus = nus / 10;
        while (nus--) {
            simple_delay10_us ();
        }
    }
}

/**
  * @brief  Generate a delay (in ms)
  * @param  nms: ms to be delayed
  * @retval None
  */
void _delay_ms (u16 nms)
{
    u32 temp;
    u16 ms;
    if (using_delay == 0) {
        using_delay = 1;
        while (nms) {

            ms = (nms > 1000) ? 1000 : nms;

            SysTick->LOAD = 9000 * ms;
            SysTick->VAL = 0x00;
            SysTick->CTRL = 0x01;
            do {
                temp = SysTick->CTRL;
            }
            while ((temp & 0x01) && (!(temp & (1 << 16))));
            SysTick->CTRL = 0x00;
            SysTick->VAL = 0x00;

            nms -= ms;
        }
        using_delay = 0;
    }
    else {
        while (nms--) {
            simple_delay1_ms ();
        }

    }
}

/**
  * @brief  A single 10us delay written in assembly
  * @param  None
  * @retval None
  */
void simple_delay10_us ()
{
    asm volatile("MOV    R0, #115    \n\t"
        "loop:              \n\t"
        "SUB    R0, R0, #1  \n\t"
        "CMP    R0, #0      \n\t"
        "BNE    loop        \n\t"
        "BX     LR          \n\t");
}

/**
  * @brief  A single 1ms delay
  * @param  None
  * @retval None
  */
void simple_delay1_ms ()
{
    u8 i = 0;
    for (i = 0; i < 100; i++)
        simple_delay10_us ();
}