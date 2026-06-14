idf_component_register(SRCS "reader.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_sdspi sdmmc vfs fatfs)
