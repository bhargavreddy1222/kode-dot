#pragma once
#ifndef RTC_H
#define RTC_H
#include "esp_err.h"
#include "driver/i2c_master.h"

#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06
#define ALARM1_1_SECONDS 0x07
#define ALARM1_1_MINUTES 0x08
#define ALARM1_1_HOURS 0x09
#define ALARM1_1_DAY_DATE 0x0A
#define ALARM1_2_MINUTES 0x0B
#define ALARM1_2_HOURS 0x0C
#define ALARM1_2_DAY_DATE 0x0D
#define control 0x0E 
#define status 0x0F
#define aging_offset 0x10
#define temp_msb 0x11
#define temp_lsb 0x12

esp_err_t rtc_init(i2c_master_bus_handle_t bus_handle);
esp_err_t rtc_set_time(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
esp_err_t rtc_alarm_set(uint8_t minutes, uint8_t hours, uint8_t day_date);
esp_err_t rtc_read_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours);
esp_err_t rtc_pow_contr();


#endif
