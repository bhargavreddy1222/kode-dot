#include <stdio.h>
#include "driver/i2c_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
//mcrophone
#include "driver/i2s_std.h"
#include "esp_err.h"
#include "esp_log.h"

//sensors
#include "lsm6d.h"
#include "rtc.h"
#include "max_fuel.h"

//display
#define LCD_HOST     SPI2_HOST
#define PIN_MOSI     11
#define PIN_SCLK     12
#define PIN_CS       10
#define PIN_DC       13
#define PIN_RST      14
#define PIN_BLK      15

#define LCD_W        240
#define LCD_H        240
#define LCD_BUF_SIZE (LCD_W * 20)  // 20 lines at a time

//ui declarations
lv_obj_t *scr_watchface;
lv_obj_t *scr_applist;
lv_obj_t *scr_settings;

// LVGL is already initialized by your display driver
// Get the active screen
lv_obj_t *scr = lv_scr_act();

void display_main()
{
    // --- 1. Init SPI bus ---
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = -1,           // not needed for display
        .sclk_io_num = PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_BUF_SIZE * 2,
    };
    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);

    // --- 2. Init LCD panel IO (SPI) ---
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num = PIN_DC,
        .cs_gpio_num = PIN_CS,
        .pclk_hz = 40 * 1000 * 1000,   // 40 MHz
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_cfg, &io_handle);

    // --- 3. Init ST7789 panel ---
    esp_lcd_panel_handle_t panel_handle;
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = PIN_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &panel_handle);

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);  // ST7789 usually needs this
    esp_lcd_panel_set_gap(panel_handle, 0, 0);
    esp_lcd_panel_disp_on_off(panel_handle, true);

    // --- 4. Backlight ON ---
    gpio_set_direction(PIN_BLK, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_BLK, 1);

    // --- 5. Init LVGL port ---
    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_BUF_SIZE,
        .double_buffer = true,
        .hres = LCD_W,
        .vres = LCD_H,
        .monochrome = false,
        .rotation = { .swap_xy = false, .mirror_x = false, .mirror_y = false },
        .flags = { .buff_dma = true },
    };
    (void)lvgl_port_add_disp(&disp_cfg);  // if you don't need the return value

    // --- 6. Draw something ---
    lvgl_port_lock(0);
    
}

void display_ui_init(void){
   scr_watchface = lv_obj_create(NULL);
   scr_applist = lv_obj_create(NULL);
   scr_settings = lv_obj_create(NULL);

   create_watchface(scr_watchface);
   create_applist(scr_applist);
   create_settings(scr_settings);

   lv_scr_load(scr_watchface);
}
void create_watchface(lv_obj_t *scr){
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    
    current_time = rtc_read_time(current_time);

    lv_obj_t *time = lv_label_create(scr);
    lv_label_set_text(time, current_time);//change this into real time later
    lv_obj_set_style_text_font(time, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time, lv_color_white(), 0);
    lv_obj_center(time);
    
    lv_obj_t *date = lv_label_create(scr);
    lv_label_set_text(date, "2024-06-01");//change this into real
    lv_obj_align(date, LV_ALIGN_CENTER, 0, 40);
}

void create_applist(lv_obj_t *scr){
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    const char *apps[] = {"Music", "", "iot", "Settings"};
    for(int i  =0; i < 4; i++){
       lv_obj_t *btn = lv_btn_create(scr);
       lv_obj_set_size(btn, 80, 80);
       lv_obj_set_pos(btn, (i % 2) * 85 + 10, (i / 2) * 85 + 10);


       lv_obj_t *label = lv_label_create(btn);
       lv_label_set_text(label, apps[i]);
       lv_obj_center(label);
    }
}

void app_main(void)
{
  i2c_master_bus_config_t bus_cfg = {
    .i2c_port      = I2C_NUM_0,
    .sda_io_num    = GPIO_NUM_8,
    .scl_io_num    = GPIO_NUM_9,
    .clk_source    = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;
i2c_new_master_bus(&bus_cfg, &bus_handle);

// Then pass bus_handle to each driver init
lsm_init(bus_handle);
rtc_init(bus_handle);
max_fuel_init(bus_handle);


}
