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
#include "dirent.h"


#define MOUNT_POINT "/sdcard"

//SPI pins
#define CS_GPIO 10
#define MOSI_GPIO 11
#define CLK_GPIO 12
#define MISO_GPIO 13

//For playlists
char trackList[16][64];
int trackCount = 0;

//For songs
char songList[50][64];
int songCount = 0;

//Function will initialize SPI bus along with SDSPI.
void initialize(){
    esp_err_t ret;

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

void list_trackslist(){
    DIR *dir = opendir(MOUNT_POINT);
    
    if (dir == NULL) {
        printf("directory null -> mount failure");
        return;
    }
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        strcpy(trackList[trackCount], entry->d_name);
        trackCount++;
        
        // printf("%s\n", entry->d_name);
    }

    for(int i = 0; i < trackCount; i++){
        printf("%s copied into playlists\n", trackList[i]);
    }
}

void seek_track(const char *trackName){
    char path[70] = MOUNT_POINT;
    strcat(path, trackName);

    DIR *dir = opendir(path);
    if (trackName == NULL){
        printf("Track could not be found");
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        printf("%s\n", entry->d_name);
    }
}

void clear_songs(void){
    songCount = 0;
}

void select_song(const char *song){

}

void run(){
    list_trackslist();
    seek_track("/Disc 1");
}