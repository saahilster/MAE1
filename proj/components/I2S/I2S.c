#include <stdio.h>
#include "I2S.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_check.h"

//TODO define these
#define DUPLEX_MODE 0
#define WS 0
#define BCLK 0
#define MCLK 0
#define DIN 0
#define DOUT 0

//32 kb
#define BUFF_SIZE 32768 

//i2s transmitter
static i2s_chan_handle_t tx_handle;
int32_t src_buff;
size_t buff_size;
size_t bytes_read;


i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_new_channel(&chan_cfg, &tx_handle, NULL);
i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(4800),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
    .gpio_cfg = {
        .mclk = MCLK,
        .bclk = BCLK,
        .ws = WS,
        .dout = DOUT,
        .din = DIN,
        .invert_flags = {
            .mclk_inv = false,
            .bclk_inv = false,
            .ws_inv = false,
        },
    },    
};

i2s_channel_init_std_mode(tx_handle, &std_cfg);
i2s_channel_enable(tx_handle);

i2s_channel_write(tx_handle, src_buff, buff_size, bytes_read, 1000);

void transmit(void)
{

}
