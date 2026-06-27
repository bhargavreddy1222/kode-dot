idf_component_register(SRCS "rtc.c"
                    INCLUDE_DIRS "include"
                    REQUIRES log esp_driver_i2c)
