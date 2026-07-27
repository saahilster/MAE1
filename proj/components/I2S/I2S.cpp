#include <stdio.h>
#include "I2S.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

// TODO define these
#define DUPLEX_MODE
#define WS GPIO_NUM_40
#define BCLK GPIO_NUM_42
// #define MCLK GPIO_NUM_1
#define DOUT GPIO_NUM_41
    
// 32 kb
#define BUFF_SIZE 32768

// i2s transmitter
static i2s_chan_handle_t tx_handle;

void initialize_bus()
{
    static const char *TAG = "I2S";
    esp_err_t error;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);

    error = i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "New channel creation failure: %s\n", esp_err_to_name(error));
        return;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = BCLK,
            .ws = WS,
            .dout = DOUT,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    error = i2s_channel_init_std_mode(tx_handle, &std_cfg);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "channel std initialization failure: %s\n", esp_err_to_name(error));
        return;
    }
    error = i2s_channel_enable(tx_handle);
    if(error != ESP_OK){
        ESP_LOGE(TAG, "channel enabling failure: %s\n", esp_err_to_name(error));
        return;
    }

    ESP_LOGI(TAG,"I2S bus successfully initialized\n");
}

void transmit(const void* input)
{
    static const char *TAG = "transmission";
    esp_err_t error;
    error = i2s_channel_write(tx_handle, input, BUFF_SIZE, NULL, 1000);

    if(error != ESP_OK){
        ESP_LOGE(TAG, "transmission failure: %s\n", esp_err_to_name(error));
        return;
    }

    ESP_LOGI(TAG, "transmission success");
}
