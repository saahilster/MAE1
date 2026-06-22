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
#include <string>
#include <vector>

#define MOUNT_POINT "/sdcard"

//SPI pins
#define CS_GPIO GPIO_NUM_10
#define MOSI_GPIO GPIO_NUM_11
#define CLK_GPIO GPIO_NUM_12
#define MISO_GPIO GPIO_NUM_13

using namespace std;

//For playlists
char trackList[16][64];
int trackCount = 0;

//For songs
vector<string> songList(50);

//Function will initialize SPI bus along with SDSPI.
void initialize(){
    esp_err_t ret;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // Don't override max_freq_khz — let it use the default

    spi_bus_config_t spi_bus = {};

    spi_bus.mosi_io_num = MOSI_GPIO;
    spi_bus.miso_io_num = MISO_GPIO;
    spi_bus.sclk_io_num = CLK_GPIO;

    spi_bus.quadwp_io_num = -1;
    spi_bus.quadhd_io_num = -1;

    spi_bus.data4_io_num = -1;
    spi_bus.data5_io_num = -1;
    spi_bus.data6_io_num = -1;
    spi_bus.data7_io_num = -1;

    spi_bus.max_transfer_sz = 4000;
    spi_bus.data_io_default_level = false;
    spi_bus.flags = 0;
    spi_bus.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    spi_bus.intr_flags = 0;

    ret = spi_bus_initialize(
        static_cast<spi_host_device_t>(host.slot), 
        &spi_bus,
        SDSPI_DEFAULT_DMA);

    if (ret != ESP_OK){
        printf("spi bus failed to initialize\n");
        printf(esp_err_to_name(ret));
        return;
    }

    printf("SPI initializing complete. Attempting to initialize SDSPI.\n");
    sdspi_device_config_t config = SDSPI_DEVICE_CONFIG_DEFAULT();
    config.host_id = static_cast<spi_host_device_t>(host.slot);
    config.gpio_cs = CS_GPIO;

    esp_vfs_fat_mount_config_t mount_config = {};

    mount_config.format_if_mount_failed = true;
    mount_config.max_files = 5;
    mount_config.allocation_unit_size = 16 * 1024;
    mount_config.disk_status_check_enable = false;
    mount_config.use_one_fat = false;

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
        spi_bus_free(static_cast<spi_host_device_t>(host.slot));
        return;
    }
 
    printf("SD card mounted successfully at %s\n", MOUNT_POINT);
    sdmmc_card_print_info(stdout, card);
}

const char* path_finder(string albumName){
    string mount = MOUNT_POINT;
    string slash = "/";
    string path = (mount + slash + albumName);
    return path.c_str();
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

//looks through album and lists all songs then adds to songlist vector.
void seek_track(const char *trackName){
    const char *path = path_finder(trackName);

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
        string songName = entry->d_name;
        songList.push_back(songName);
        printf("%s\n added to queue", songName.c_str()); 
    }
}

//call this function when switching to another playlist.
void clear_songs(void){
    songList.clear();
}

void select_song(const char *song){

}

void run(){
    list_trackslist();
    //Test out
    seek_track("Disc 1");
}