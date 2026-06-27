#pragma once
#ifndef MAX_FUEL_H
#define MAX_FUEL_H
#include "esp_err.h"

#define VCELL_LSB   0x02
#define VCELL_MSB   0x03
#define SOC_LSB     0x04
#define SOC_MSB     0x05
#define MODE_LSB    0x06
#define MODE_MSB    0x07
#define VERSION_LSB 0x08
#define VERSION_MSB 0x09
#define CONFIG_LSB  0x97//return 0x0C
#define CONFIG_MSB  0x1C//return 0x0D
  
#define ALRT  (1 << 5)
#define SLEEP (1 << 7)



esp_err_t max_fuel_init();
esp_err_t max_fuel_read_vcell(float *vcell);
esp_err_t max_fuel_read_soc(float *soc);
esp_err_t max_fuel_mode(uint16_t *mode);
esp_err_t max_fuel_config(uint16_t *config);
#endif /* MAX_FUEL_H */