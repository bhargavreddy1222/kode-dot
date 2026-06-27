#ifndef LSM6D_H
#define LSM6D_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

// Addresses ──────────────────────────────────────────
#define LSM6DSO_ADDR       0x6A   // SA0=GND → 0x6A, SA0=VCC → 0x6B
#define WHO_AM_I_REG       0x0F
#define WHO_AM_I_VAL       0x6C   // LSM6DSO returns this

// ── Control Registers ───────────────────────────────────
#define CTRL1_XL           0x10   // Accel config
#define CTRL2_G            0x11   // Gyro config
#define CTRL3_C            0x12   // General (reset, BDU)

// ── Data Registers ──────────────────────────────────────
#define OUTX_L_G           0x22   // Gyro X low (start of 6-byte block)
#define OUTX_L_A           0x28   // Accel X low (start of 6-byte block)
#define OUT_TEMP_L         0x20   // Temp low byte

// ── Status ──────────────────────────────────────────────
#define STATUS_REG         0x1E

// ── FIFO ────────────────────────────────────────────────
#define FIFO_CTRL1         0x07
#define FIFO_CTRL2         0x08
#define FIFO_CTRL3         0x09
#define FIFO_CTRL4         0x0A
#define FIFO_STATUS1       0x3A
#define FIFO_DATA_OUT_L    0x3C

// ── Interrupts ──────────────────────────────────────────
#define INT1_CTRL          0x0D
#define INT2_CTRL          0x0E
#define ALL_INT_SRC        0x1A

// ── Wake-up ─────────────────────────────────────────────
#define WAKE_UP_SRC        0x1B
#define WAKE_SLEEP_CHANGE_IA  (1 << 6)
#define WAKE_FF_IA            (1 << 5)
#define WAKE_SLEEP_STATE      (1 << 4)
#define WAKE_WU_IA            (1 << 3)
#define WAKE_X_WU             (1 << 2)
#define WAKE_Y_WU             (1 << 1)
#define WAKE_Z_WU             (1 << 0)

// ── Tap ─────────────────────────────────────────────────
#define TAP_SRC            0x1C
#define TAP_IA             (1 << 6)
#define SINGLE_TAP         (1 << 5)
#define DOUBLE_TAP         (1 << 4)
#define TAP_SIGN           (1 << 3)
#define X_TAP              (1 << 2)
#define Y_TAP              (1 << 1)
#define Z_TAP              (1 << 0)

// ── 6D Orientation ──────────────────────────────────────
#define D6D_SRC            0x1D

typedef struct {
    bool sleep_change_ia;
    bool ff_ia;
    bool sleep_state;
    bool wu_ia;
    bool x_wu, y_wu, z_wu;
} lsm_wakeup_t;

typedef struct {
    bool tap_ia;
    bool single_tap;
    bool double_tap;
    bool tap_sign;
    bool x_tap, y_tap, z_tap;
} lsm_tap_t;

// ── Public API ───────────────────────────────────────────
esp_err_t lsm_init(i2c_master_bus_handle_t bus_handle);

// raw reads (int16 = raw ADC counts, you convert to mg/dps yourself)
esp_err_t lsm_read_accel(int16_t *x, int16_t *y, int16_t *z);
esp_err_t lsm_read_gyro(int16_t *x, int16_t *y, int16_t *z);
esp_err_t lsm_read_temp(int16_t *temp);

// events
esp_err_t lsm_read_wakeup(lsm_wakeup_t *out);
esp_err_t lsm_read_tap(lsm_tap_t *out);

#endif