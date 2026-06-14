#include <stdio.h>
#include "reader.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"


#define MOUNT_POINT "/sdcard"

//SPI pins
#define CS_GPIO 10
#define MOSI_GPIO 11
#define CLK_GPIO 12
#define MISO_GPIO 13

//Function will initialize SPI bus along with SDSPI.
void initialize(){
    esp_err_t ret;

    // Deassert CS and let the card power stabilize
    gpio_config_t cs_conf = {
        .pin_bit_mask = (1ULL << CS_GPIO),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    gpio_config(&cs_conf);
    gpio_set_level(CS_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));


    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // Don't override max_freq_khz — let it use the default

    spi_bus_config_t spi_bus = {
        .mosi_io_num = MOSI_GPIO,
        .miso_io_num = MISO_GPIO,
        .sclk_io_num = CLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &spi_bus, SDSPI_DEFAULT_DMA);

    if (ret != ESP_OK){
        printf("spi bus failed to initialize\n");
        printf(esp_err_to_name(ret));
        return;
    }

    printf("SPI initializing complete. Attempting to initialize SDSPI.\n");
    sdspi_device_config_t config = SDSPI_DEVICE_CONFIG_DEFAULT();
    config.host_id = host.slot;
    config.gpio_cs = CS_GPIO;

    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_card_t *card = NULL;

    ret = esp_vfs_fat_sdspi_mount(
        MOUNT_POINT,
        &host,
        &config,
        &mount_config,
        &card
    );

    if (ret != ESP_OK) {
        printf("Mount failed: %s\n", esp_err_to_name(ret));
        if (ret == ESP_FAIL) {
            printf("  -> Filesystem mount failed (card detected but unreadable)\n");
        } else if (ret == ESP_ERR_TIMEOUT) {
            printf("  -> Timeout: card not responding (check wiring/power)\n");
        } else if (ret == ESP_ERR_INVALID_RESPONSE) {
            printf("  -> Invalid response: card may not be in SPI mode\n");
        } else if (ret == ESP_ERR_INVALID_SIZE) {
            printf("  -> Invalid size: check allocation_unit_size\n");
        } else {
            printf("  -> Unknown error\n");
        }
        spi_bus_free(host.slot);
        return;
    }
 
    printf("SD card mounted successfully at %s\n", MOUNT_POINT);
    sdmmc_card_print_info(stdout, card);

}
