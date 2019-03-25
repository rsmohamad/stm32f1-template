#include "led.h"
u8 state = 0;

void led_init ()
{
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO | CAN_LED_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = CAN_LED_Pin;
    GPIO_Init (CAN_LED_Port, &GPIO_InitStructure);
    GPIO_ResetBits (CAN_LED_Port, CAN_LED_Pin);
}

void led_blink ()
{
    if (state)
        GPIO_SetBits (CAN_LED_Port, CAN_LED_Pin);
    else
        GPIO_ResetBits (CAN_LED_Port, CAN_LED_Pin);
    state = !state;
}

void led_off ()
{
    GPIO_ResetBits (CAN_LED_Port, CAN_LED_Pin);
    state = 0;
}

void led_on ()
{
    GPIO_SetBits (CAN_LED_Port, CAN_LED_Pin);
    state = 1;
}