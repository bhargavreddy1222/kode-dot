idf_component_register(SRCS "lsm6d.c"
    INCLUDE_DIRS "include"
    REQUIRES log esp_driver_i2c)