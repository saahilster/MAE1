#include <stdio.h>
#include "I2S.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_check.h"

//TODO define these
#define DUPLEX_MODE 0
#define WS 0
#define BCK 0
#define DIN 0
#define DOUT 0

//16 kb
#define BUFF_SIZE 16384 

//i2s transmitter
static i2s_chan_handle_t tx_chan;

void transmit(void)
{

}
