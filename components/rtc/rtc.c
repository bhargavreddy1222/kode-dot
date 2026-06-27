#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "RTC";

static i2c_master_dev_handle_t s_dev;

static esp_err_t rtc_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(s_dev, buf, 2, 50);
}
static esp_err_t rtc_read(uint8_t reg, uint8_t *out, size_t len) {
    return i2c_master_transmit_receive(s_dev, &reg, 1, out, len, 50);
}

esp_err_t rtc_init(i2c_master_bus_handle_t bus_handle) {
    // Initialize the RTC device here (e.g., set up I2C communication, configure registers)
    // This is a placeholder implementation and should be replaced with actual initialization code.
    esp_err_t ret;

    i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address  = 0x68, // Replace with your RTC's I2C address
      .scl_speed_hz    = 400000,
    };
    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_dev);
    if (ret != ESP_OK) {return ret;}

    uint8_t id = 0;
    ret = rtc_read(RTC_REG_ID, &id, 1);
    if (ret != ESP_OK || id != RTC_ID_VAL) {
        ESP_LOGE(TAG, "RTC ID check failed: got 0x%02X", id);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "RTC ID: 0x%02X", id);

  }

esp_err_t rtc_set_time(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year) 
{
    // Set the RTC time here by writing to the appropriate registers
    // This is a placeholder implementation and should be replaced with actual code to set the time.
    rtc_write(SECONDS, seconds);
    rtc_write(MINUTES, minutes);
    rtc_write(HOURS, hours);
    if(hours >= 13){
      rtc_write(HOURS, HOURS &= ~(1 << 6)); // Set 12-hour format bit
    }
    else{
      rtc_write(HOURS, HOURS |= (1 << 5)); // set pm bit
    }
    rtc_write(DAY, day);
    rtc_write(DATE, date);
    rtc_write(MONTH, month);
    rtc_write(YEAR, year);
    // let the user sets the time, so we can test reading it back
    return ESP_OK;
  }
esp_err_t rtc_alarm_set(uint8_t minutes, uint8_t hours, uint8_t day_date) {
    // Set the RTC alarm here by writing to the appropriate registers
    // This is a placeholder implementation and should be replaced with actual code to set the alarm.
    rtc_write(ALARM1_1_MINUTES, minutes);
    rtc_write(ALARM1_1_HOURS, hours);
    rtc_write(ALARM1_1_DAY_DATE, day_date);
    rtc_write(ALARM1_1_DAY_DATE,ALARM1_1_DAY_DATE |= (1 << 6));

    //has to set control for alarm bit 0,1,2

    return ESP_OK;
  }
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
}rtc_time_t;
  
esp_err_t rtc_read_time(rtc_time_t *current_time, char *out_str, size_t str_size) {
    // Read the RTC time here by reading from the appropriate registers
    // This is a placeholder implementation and should be replaced with actual code to read the time.
  
    rtc_read(SECONDS, &current_time->seconds, 1);
    rtc_read(MINUTES, &current_time->minutes, 1);
    rtc_read(HOURS, &current_time->hours, 1);
    rtc_read(DAY, &current_time->day, 1);
    rtc_read(DATE, &current_time->date, 1);
    rtc_read(MONTH, &current_time->month, 1);
    rtc_read(YEAR, &current_time->year, 1);
    if(current_time ==NULL || out_str == NULL || str_size == 0){
      return ESP_ERR_INVALID_ARG;
    }
    snprintf(out_str, str_size, "%02d:%02d:%02d %02d/%02d/%02d", current_time->hours, current_time->minutes, current_time->seconds, current_time->date, current_time->month, current_time->year);
    return ESP_OK;
}