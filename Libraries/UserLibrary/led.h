#ifndef __LEDS_H
#define __LEDS_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#define CAN_LED_Port GPIOA
#define CAN_LED_Pin  GPIO_Pin_1
#define CAN_LED_RCC  RCC_APB2Periph_GPIOA

void led_init (void);
void led_blink (void);
void led_off (void);
void led_on (void);

#endif