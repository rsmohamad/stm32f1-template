#include "main.h"

CAN_MESSAGE txMsg;
u16 time;

void can_receive (CanRxMsg msg)
{
    txMsg.id = msg.StdId;
    txMsg.length = msg.DLC;
    txMsg.data[0] = msg.Data[0];
    can_tx_enqueue (txMsg);
}

void init_all (void)
{
    __enable_irq ();
    can_init ();
    can_rx_init ();
    can_rx_add_filter (0x640, CAN_RX_MASK_EXACT, &can_receive);

    ticks_init ();
    led_init ();
    led_off ();
    led_on ();
}

int main (void)
{
    init_all ();

    while (1) {
        if (time != get_ms_ticks ()) {
            time = get_ms_ticks ();

            if (time % 100 == 0) {
                led_blink ();
            }
        }
    };

    return 0;
}
