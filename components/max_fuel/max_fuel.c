#include <stdio.h>
#include "max_fuel.h"
#include "driver/i2c_master.h"
#include "esp_log.h"



static i2c_master_dev_handle_t m_dev;

static esp_err_t max_fuel_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(m_dev, buf, 2, 50);
}
static esp_err_t max_fuel_read(uint8_t reg, uint8_t *out, size_t len)
{
    return i2c_master_transmit_receive(m_dev, &reg, 1, out, len, 50);
}   

esp_err_t max_fuel_init(i2c_master_bus_handle_t bus_handle)
{
  esp_err_t ret;
  i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = CONFIG_MSB << 8 | CONFIG_LSB,
        .scl_speed_hz    = 400000,
    };
 ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &m_dev);

  if(ret != ESP_OK) return ret;

  uint8_t id = 0;
  ret = max_fuel_read(VERSION_LSB, &id, 1);
  if (ret != ESP_OK || id != 0x10) 
    {
        ESP_LOGE("MAX_FUEL", "VERSION check failed: got 0x%02X", id);
        return ESP_ERR_NOT_FOUND;  
      }

  }
esp_err_t max_fuel_read_soc(float *fuel_level){
  uint8_t raw_l[2] , raw_m[2];
  esp_err_t ret = max_fuel_read(SOC_LSB, raw_l, 2);
  if (ret != ESP_OK) return ret;
  esp_err_t ret_1 = max_fuel_read(SOC_MSB, raw_m, 2);
  if (ret_1 != ESP_OK) return ret_1;
  

  uint16_t soc = (raw_m[1] << 8) | raw_l[0];
  *fuel_level = soc / 256.0f * 100.0f; // Convert to percentage
  return ESP_OK;

}
esp_err_t max_fuel_read_vcell(float *vcell){
  uint8_t raw_l[2] , raw_m[2];
  esp_err_t ret = max_fuel_read(VCELL_LSB, raw_l, 2);
  if (ret != ESP_OK) return ret;
  esp_err_t ret_1 = max_fuel_read(VCELL_MSB, raw_m, 2);
  if (ret_1 != ESP_OK) return ret_1;
  

  uint16_t vcell_raw = (raw_m[1] << 8) | raw_l[0];
  *vcell = vcell_raw * 0.00125f; // Convert to volts
  return ESP_OK;

}