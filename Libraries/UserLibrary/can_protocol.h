#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

#include <stdio.h>
#include <stdlib.h> 
#include <stm32f10x.h>
#include "stm32f10x_can.h"
#include "misc.h"
#include "ticks.h"

/*** CAN Config ***/
#define    CANn                            CAN1
#define CAN_RCC                        RCC_APB1Periph_CAN1

#define    CAN_Rx_GPIO_Pin        GPIO_Pin_11
#define    CAN_Tx_GPIO_Pin        GPIO_Pin_12
#define    CAN_GPIO_PORT            GPIOA
#define CAN_GPIO_RCC            RCC_APB2Periph_GPIOA

/*** CAN TX CONST ***/
#define CAN_TX_QUEUE_MAX_SIZE                200
#define    CAN_TX_IRQn                                    USB_HP_CAN1_TX_IRQn
#define CAN_TX_IRQHander                        void USB_HP_CAN1_TX_IRQHandler(void)

/*** CAN RX CONST ***/
#define CAN_Rx_IRQn                                    USB_LP_CAN1_RX0_IRQn
#define    CAN_Rx_IRQHandler                        void USB_LP_CAN1_RX0_IRQHandler(void)
#define    CAN_RX_FILTER_LIMIT        28        // The number of filters can be applied at most


/*** X = the ID bit that must be equal 	***/
/*** ? = the ID bit that can varies 		***/
/***   11-bit ID   	(example range) 		***/
#define CAN_RX_MASK_EXACT                        0x7FF        /*** XXX XXXX XXXX	(Exactly same ID)		***/
#define CAN_RX_MASK_DIGIT_0_F                0x7F0        /*** XXX XXXX ???? 	(0xAB0 - 0xABF) 		***/
#define    CAN_RX_MASK_DIGIT_0_7                0x7F8        /*** XXX XXXX X??? 	(0xAB0 - 0xAB7)			***/
#define    CAN_RX_MASK_DIGIT_0_3                0x7FC        /*** XXX XXXX XX?? 	(0xAB0 - 0xAB3)			***/
#define    CAN_RX_MASK_DIGIT_0_1                0x7FE        /*** XXX XXXX XXX? 	(0xAB0 - 0xAB1)			***/


typedef struct
{
    u32 id;            /*** 11-bit ID: 0x000 to 0x7FF ***/
    u8 length;    /*** 0 to 8 ***/
    u8 data[8];
} CAN_MESSAGE;

typedef struct
{
    u16 head;                        /*** Current head of queue ***/
    u16 tail;                        /*** Current tail of queue ***/
    const u16 length;    /*** Length of queue ***/
    volatile CAN_MESSAGE *queue;        /*** The can message queue (array) ***/
} CAN_QUEUE;

void can_init (void);
//u8 can_tx(CanTxMsg msg);

/*** CAN Tx ***/
u16 can_tx_queue_head (void);
u16 can_tx_queue_tail (void);
u16 can_tx_queue_size (void);
u8 can_tx_queue_empty (void);
u8 can_empty_mailbox (void);
u8 can_tx_enqueue (CAN_MESSAGE msg);        // <--- The main function for CAN transmission
u8 can_tx_dequeue (void);                            // <--- To be called through interrupt
void can_tx_queue_clear (void);

/*** CAN Rx ***/
void can_rx_init (void);
void can_rx_add_filter (u16 id, u16 mask, void (*handler) (CanRxMsg msg));
u32 can_get_rx_count (void);
CAN_MESSAGE can_get_recent_rx (void);

/*** Protocol Encoding / Decoding ***/
u8 one_to_n_bytes (s32 num, u8 n);            // Encode
s32 n_bytes_to_one (u8 *array, u8 n);    // Decode

#endif /* __CAN_PROTOCOL_H */