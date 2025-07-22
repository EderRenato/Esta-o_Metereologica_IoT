#ifndef AHT20_H
#define AHT20_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define AHT20_I2C_ADDR 0x38

// Comandos do AHT20
#define AHT20_INIT_CMD 0xBE
#define AHT20_TRIGGER_MEASUREMENT_CMD 0xAC
#define AHT20_SOFT_RESET_CMD 0xBA

// Estados do sensor
#define AHT20_STATUS_BUSY 0x80
#define AHT20_STATUS_CALIBRATED 0x08

typedef struct {
    i2c_inst_t *i2c;
    uint8_t address;
    bool initialized;
} aht20_t;

bool aht20_init(aht20_t *aht, i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate);


bool aht20_read(aht20_t *aht, float *temperature, float *humidity);


bool aht20_soft_reset(aht20_t *aht);

#endif // AHT20_H