#include "include/lsm6d.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "LSM6D";

// One global handle — the whole file uses this
static i2c_master_dev_handle_t s_dev;

static esp_err_t lsm_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(s_dev, buf, 2, 50);
}

// THIS is what you were calling as lsm_read() — now it actually exists
static esp_err_t lsm_read(uint8_t reg, uint8_t *out, size_t len)
{
    return i2c_master_transmit_receive(s_dev, &reg, 1, out, len, 50);
}

// ── Init ────────────────────────────────────────────────

esp_err_t lsm_init(i2c_master_bus_handle_t bus_handle)
{
    esp_err_t ret;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = LSM6DSO_ADDR,
        .scl_speed_hz    = 400000,
    };
    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_dev);
    if (ret != ESP_OK) return ret;

    // WHO_AM_I check
    uint8_t id = 0;
    ret = lsm_read(WHO_AM_I_REG, &id, 1);
    if (ret != ESP_OK || id != WHO_AM_I_VAL) {
        ESP_LOGE(TAG, "WHO_AM_I failed: got 0x%02X", id);
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "Found LSM6DSO (0x%02X)", id);

    // Software reset
    lsm_write(CTRL3_C, 0x01);
    vTaskDelay(pdMS_TO_TICKS(10));

    // BDU + auto-increment on
    lsm_write(CTRL3_C, 0x44);

    // Accel: 104Hz, ±4g
    lsm_write(CTRL1_XL, 0x48);

    // Gyro: 104Hz, ±500dps
    lsm_write(CTRL2_G, 0x44);

    ESP_LOGI(TAG, "Init OK");
    return ESP_OK;
}

// ── Data reads ──────────────────────────────────────────

esp_err_t lsm_read_accel(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t raw[6];
    esp_err_t ret = lsm_read(OUTX_L_A, raw, 6);
    if (ret != ESP_OK) return ret;

    *x = (int16_t)((raw[1] << 8) | raw[0]);
    *y = (int16_t)((raw[3] << 8) | raw[2]);
    *z = (int16_t)((raw[5] << 8) | raw[4]);
    return ESP_OK;
}

esp_err_t lsm_read_gyro(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t raw[6];
    esp_err_t ret = lsm_read(OUTX_L_G, raw, 6);
    if (ret != ESP_OK) return ret;

    *x = (int16_t)((raw[1] << 8) | raw[0]);
    *y = (int16_t)((raw[3] << 8) | raw[2]);
    *z = (int16_t)((raw[5] << 8) | raw[4]);
    return ESP_OK;
}

esp_err_t lsm_read_temp(int16_t *temp)
{
    uint8_t raw[2];
    esp_err_t ret = lsm_read(OUT_TEMP_L, raw, 2);
    if (ret != ESP_OK) return ret;

    *temp = (int16_t)((raw[1] << 8) | raw[0]);
    return ESP_OK;
}

// ── Events ──────────────────────────────────────────────

esp_err_t lsm_read_wakeup(lsm_wakeup_t *out)
{
    uint8_t data;
    esp_err_t ret = lsm_read(WAKE_UP_SRC, &data, 1);
    if (ret != ESP_OK) return ret;

    out->sleep_change_ia = !!(data & WAKE_SLEEP_CHANGE_IA);
    out->ff_ia           = !!(data & WAKE_FF_IA);
    out->sleep_state     = !!(data & WAKE_SLEEP_STATE);
    out->wu_ia           = !!(data & WAKE_WU_IA);
    out->x_wu            = !!(data & WAKE_X_WU);
    out->y_wu            = !!(data & WAKE_Y_WU);
    out->z_wu            = !!(data & WAKE_Z_WU);
    return ESP_OK;
}

esp_err_t lsm_read_tap(lsm_tap_t *out)
{
    uint8_t data;
    esp_err_t ret = lsm_read(TAP_SRC, &data, 1);
    if (ret != ESP_OK) return ret;

    out->tap_ia    = !!(data & TAP_IA);
    out->single_tap = !!(data & SINGLE_TAP);
    out->double_tap = !!(data & DOUBLE_TAP);
    out->tap_sign  = !!(data & TAP_SIGN);
    out->x_tap     = !!(data & X_TAP);
    out->y_tap     = !!(data & Y_TAP);
    out->z_tap     = !!(data & Z_TAP);
    return ESP_OK;
}