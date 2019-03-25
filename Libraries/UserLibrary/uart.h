#ifndef __UART_H
#define __UART_H

#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum
{
    COM_NULL = -1,   //disabled
    COM1 = 0,        //usart1
    COM3 = 1,        //usart3
} COM_TypeDef;

typedef void on_receive_listener (const uint8_t byte);

void uart_init (COM_TypeDef COM, u32 br);
void uart_tx_byte (COM_TypeDef COM, const char data);
void uart_tx (COM_TypeDef COM, const char *tx_buf, ...);
void uart_interrupt (COM_TypeDef COM);
void uart_interrupt_init (COM_TypeDef COM, on_receive_listener *listener);

#endif