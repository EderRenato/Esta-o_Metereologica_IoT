#ifndef BMP280_H
#define BMP280_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define BMP280_I2C_ADDR 0x76

// Oversampling settings
#define BMP280_OS_NONE    0
#define BMP280_OS_1X      1
#define BMP280_OS_2X      2
#define BMP280_OS_4X      3
#define BMP280_OS_8X      4
#define BMP280_OS_16X     5

// Mode settings
#define BMP280_SLEEP_MODE  0
#define BMP280_FORCED_MODE 1
#define BMP280_NORMAL_MODE 3

// Filter settings
#define BMP280_FILTER_OFF  0
#define BMP280_FILTER_2    1
#define BMP280_FILTER_4    2
#define BMP280_FILTER_8    3
#define BMP280_FILTER_16   4

// Standby time settings (ms)
#define BMP280_STANDBY_0_5   0
#define BMP280_STANDBY_62_5  1
#define BMP280_STANDBY_125   2
#define BMP280_STANDBY_250   3
#define BMP280_STANDBY_500   4
#define BMP280_STANDBY_1000  5
#define BMP280_STANDBY_2000  6
#define BMP280_STANDBY_4000  7

// Registradores BMP280
#define BMP280_REG_CALIB     0x88
#define BMP280_REG_ID        0xD0
#define BMP280_REG_RESET     0xE0
#define BMP280_REG_STATUS    0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_PRESS_LSB 0xF8
#define BMP280_REG_PRESS_XLSB 0xF9
#define BMP280_REG_TEMP_MSB  0xFA
#define BMP280_REG_TEMP_LSB  0xFB
#define BMP280_REG_TEMP_XLSB 0xFC
typedef struct {
    i2c_inst_t *i2c;
    uint8_t address;
    float sea_level_pressure;
    
    // Calibration data
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} bmp280_t;

bool bmp280_init(bmp280_t *sensor, i2c_inst_t *i2c, uint8_t address);
bool bmp280_config(bmp280_t *sensor, uint8_t osrs_t, uint8_t osrs_p, uint8_t mode, 
                  uint8_t filter, uint8_t standby);
bool bmp280_read(bmp280_t *sensor, float *temperature, float *pressure, float *altitude);
void bmp280_set_sea_level_pressure(bmp280_t *sensor, float pressure);

#endif // BMP280_H