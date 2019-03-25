#include "uart.h"

USART_TypeDef *com_usart[2] = {USART1, USART3};
uc32 com_tx_port_clk[2] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB};
uc32 com_rx_port_clk[2] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB};
uc32 com_usart_clk[2] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART3};
GPIO_TypeDef *com_tx_port[2] = {GPIOA, GPIOB};
GPIO_TypeDef *com_rx_port[2] = {GPIOA, GPIOB};
uc16 com_tx_pin[2] = {GPIO_Pin_9, GPIO_Pin_10};
uc16 com_rx_pin[2] = {GPIO_Pin_10, GPIO_Pin_11};
uc16 com_irq[2] = {USART1_IRQn, USART3_IRQn};

on_receive_listener *uart_rx_listener[2];
u8 has_uart_rx_listener[2] = {0};

void uart_init (COM_TypeDef COM, u32 br)
{
    RCC_APB2PeriphClockCmd (com_rx_port_clk[COM] | com_rx_port_clk[COM] | RCC_APB2Periph_AFIO, ENABLE);
    if (COM == COM1)
        RCC_APB2PeriphClockCmd (com_usart_clk[COM], ENABLE);
    else
        RCC_APB1PeriphClockCmd (com_usart_clk[COM], ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = com_tx_pin[COM];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (com_tx_port[COM], &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = com_rx_pin[COM];
    GPIO_Init (com_rx_port[COM], &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = br;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init (com_usart[COM], &USART_InitStructure);
    USART_ITConfig (com_usart[COM], USART_IT_RXNE, ENABLE);
    USART_Cmd (com_usart[COM], ENABLE);
}

void uart_tx_byte (COM_TypeDef COM, char data)
{
    while (USART_GetFlagStatus (com_usart[COM], USART_FLAG_TC) == RESET);
    USART_SendData (com_usart[COM], (u16) data);
}

void uart_tx (COM_TypeDef COM, const char *tx_buf, ...)
{
    va_list arglist;
    char buf[255], *fp;

    va_start(arglist, tx_buf);
    vsprintf (buf, tx_buf, arglist);
    va_end(arglist);

    fp = buf;
    while (*fp)
        uart_tx_byte (COM, *fp++);
}

void uart_interrupt (COM_TypeDef COM)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    #ifdef VECT_TAB_RAM
    NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
    #else
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, 0x0);
    #endif

    NVIC_InitStructure.NVIC_IRQChannel = com_irq[COM];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    USART_ITConfig (com_usart[COM], USART_IT_RXNE, ENABLE);
}

void uart_interrupt_init (COM_TypeDef COM, on_receive_listener *listener)
{
    uart_rx_listener[COM] = listener;
    has_uart_rx_listener[COM] = 1;
    uart_interrupt (COM);
}

void USART1_IRQHandler (void)
{
    if (USART_GetITStatus (USART1, USART_IT_RXNE) != RESET) {
        if (has_uart_rx_listener[COM1])
            (*uart_rx_listener[COM1]) (USART_ReceiveData (USART1));
        USART_ClearITPendingBit (USART1, USART_IT_RXNE);
    }
}

void USART3_IRQHandler (void)
{
    if (USART_GetITStatus (USART3, USART_IT_RXNE) != RESET) {
        if (has_uart_rx_listener[COM3])
            (*uart_rx_listener[COM3]) (USART_ReceiveData (USART3));
        USART_ClearITPendingBit (USART3, USART_IT_RXNE);
    }
}