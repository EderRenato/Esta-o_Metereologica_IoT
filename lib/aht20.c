#include "aht20.h"
#include <stdio.h>

bool aht20_init(aht20_t *aht, i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate) {
    aht->i2c = i2c;
    aht->address = AHT20_I2C_ADDR;
    aht->initialized = false;

    // Inicializa I2C
    i2c_init(aht->i2c, baudrate);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    // Verifica se o dispositivo está presente
    uint8_t buf[1];
    int ret = i2c_read_blocking(aht->i2c, aht->address, buf, 1, false);
    if (ret < 0) {
        return false;
    }

    // Envia comando de inicialização
    uint8_t init_cmd[3] = {AHT20_INIT_CMD, 0x08, 0x00};
    ret = i2c_write_blocking(aht->i2c, aht->address, init_cmd, 3, false);
    if (ret < 0) {
        return false;
    }

    sleep_ms(10); // Tempo para inicialização

    // Verifica se o sensor está calibrado
    uint8_t status;
    ret = i2c_read_blocking(aht->i2c, aht->address, &status, 1, false);
    if (ret < 0 || !(status & AHT20_STATUS_CALIBRATED)) {
        return false;
    }

    aht->initialized = true;
    return true;
}

bool aht20_read(aht20_t *aht, float *temperature, float *humidity) {
    if (!aht->initialized) return false;

    // Envia comando para iniciar medição
    uint8_t cmd[3] = {AHT20_TRIGGER_MEASUREMENT_CMD, 0x33, 0x00};
    int ret = i2c_write_blocking(aht->i2c, aht->address, cmd, 3, false);
    if (ret < 0) {
        return false;
    }

    // Espera até que a medição esteja pronta
    uint8_t status;
    do {
        sleep_ms(10);
        ret = i2c_read_blocking(aht->i2c, aht->address, &status, 1, false);
        if (ret < 0) {
            return false;
        }
    } while (status & AHT20_STATUS_BUSY);

    // Lê os dados brutos (6 bytes: status + umidade + temperatura)
    uint8_t data[6];
    ret = i2c_read_blocking(aht->i2c, aht->address, data, 6, false);
    if (ret < 0) {
        return false;
    }

    // Converte os dados para valores reais
    uint32_t raw_humidity = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    *humidity = (float)raw_humidity * 100.0 / (1 << 20);
    *temperature = (float)raw_temp * 200.0 / (1 << 20) - 50.0;

    return true;
}

bool aht20_soft_reset(aht20_t *aht) {
    uint8_t cmd = AHT20_SOFT_RESET_CMD;
    int ret = i2c_write_blocking(aht->i2c, aht->address, &cmd, 1, false);
    if (ret < 0) {
        return false;
    }
    sleep_ms(20); // Tempo para reset
    return true;
}