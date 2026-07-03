# MAE1
Compact Audio Player that plays songs/audio from a micro-SD card. This device is designed for people who love physical media, and want high quality audio connected to a keychain. I built this device because of how impactful music is to me. I always think it's fascinating that music has borne such an impact on humanity, and I'd be over the moon to know that my invention has given people a unique way to experience the gift of music.

## Tech Stack

### Hardware
- MCU: ESP32-S3 N16R8
- DAC: PCM5102A
- AMP: TDA1308
- LDO: ME6211C33M5G-N (custom PCBA)
- Buck Converter: TPS629210DRLR
- SD Card Reader: WWZMDiB micro sd-card reader

### Software
- ESP-IDF v6.0.1 CLI
- micro-flac
- esp_heap_caps
- vfs
- sdmmc

## Features
- GUI that displays album art, multiple playlists, and song progression
- 32-bit bit depth audio
- Stereo sound
- FLAC support

## Experience Gained
- Embedded systems programming with ESP-IDF
- C and C++ programming
- Systems design
- Allocating RAM for certain tasks, such as storing FLAC data
