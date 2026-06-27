idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_driver_uart
        esp_driver_i2c
        esp_driver_spi
        esp_driver_gpio
        esp_driver_i2s
        esp_lcd
        esp_lvgl_port
        lvgl
        log
        lsm6d
        rtc
        max_fuel
        bq
)