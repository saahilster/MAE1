idf_component_register(SRCS "I2S.c"
                    PRIV_REQUIRES esp_driver_i2s esp_driver_gpio
                    INCLUDE_DIRS "include")
