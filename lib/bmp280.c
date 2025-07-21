#include "bmp280.h"
#include <math.h>

bool bmp280_init(bmp280_t *sensor, i2c_inst_t *i2c, uint8_t address) {
    sensor->i2c = i2c;
    sensor->address = address;
    sensor->sea_level_pressure = 101325.0f; // Pressão padrão ao nível do mar
    
    // Verifica ID do sensor (0x58 para BMP280)
    uint8_t reg = BMP280_REG_ID;
    uint8_t id;
    i2c_write_blocking(sensor->i2c, sensor->address, &reg, 1, true);
    i2c_read_blocking(sensor->i2c, sensor->address, &id, 1, false);
    
    if(id != 0x58) {
        return false;
    }
    
    // Lê dados de calibração (24 bytes)
    reg = BMP280_REG_CALIB;
    uint8_t calib_data[24];
    i2c_write_blocking(sensor->i2c, sensor->address, &reg, 1, true);
    i2c_read_blocking(sensor->i2c, sensor->address, calib_data, 24, false);
    
    // Converte dados de calibração
    sensor->dig_T1 = (uint16_t)((calib_data[1] << 8) | calib_data[0]);
    sensor->dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    sensor->dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);
    
    sensor->dig_P1 = (uint16_t)((calib_data[7] << 8) | calib_data[6]);
    sensor->dig_P2 = (int16_t)((calib_data[9] << 8) | calib_data[8]);
    sensor->dig_P3 = (int16_t)((calib_data[11] << 8) | calib_data[10]);
    sensor->dig_P4 = (int16_t)((calib_data[13] << 8) | calib_data[12]);
    sensor->dig_P5 = (int16_t)((calib_data[15] << 8) | calib_data[14]);
    sensor->dig_P6 = (int16_t)((calib_data[17] << 8) | calib_data[16]);
    sensor->dig_P7 = (int16_t)((calib_data[19] << 8) | calib_data[18]);
    sensor->dig_P8 = (int16_t)((calib_data[21] << 8) | calib_data[20]);
    sensor->dig_P9 = (int16_t)((calib_data[23] << 8) | calib_data[22]);
    
    return true;
}

bool bmp280_config(bmp280_t *sensor, uint8_t osrs_t, uint8_t osrs_p, uint8_t mode, 
                  uint8_t filter, uint8_t standby) {
    // Configura o registrador CTRL_MEAS
    uint8_t ctrl_meas = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t data[2] = {BMP280_REG_CTRL_MEAS, ctrl_meas};
    if(i2c_write_blocking(sensor->i2c, sensor->address, data, 2, false) != 2) {
        return false;
    }
    
    // Configura o registrador CONFIG
    uint8_t config = (standby << 5) | (filter << 2);
    data[0] = BMP280_REG_CONFIG;
    data[1] = config;
    if(i2c_write_blocking(sensor->i2c, sensor->address, data, 2, false) != 2) {
        return false;
    }
    
    return true;
}

bool bmp280_read(bmp280_t *sensor, float *temperature, float *pressure, float *altitude) {
    // Lê dados brutos (6 bytes: pressão msb, lsb, xlsb + temperatura msb, lsb, xlsb)
    uint8_t reg = BMP280_REG_PRESS_MSB;
    uint8_t data[6];
    i2c_write_blocking(sensor->i2c, sensor->address, &reg, 1, true);
    if(i2c_read_blocking(sensor->i2c, sensor->address, data, 6, false) != 6) {
        return false;
    }
    
    // Converte dados brutos de pressão (20 bits)
    int32_t adc_P = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4));
    
    // Converte dados brutos de temperatura (20 bits)
    int32_t adc_T = (int32_t)(((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4));
    
    // Cálculo da temperatura compensada
    int32_t var1, var2, t_fine;
    var1 = ((((adc_T >> 3) - ((int32_t)sensor->dig_T1 << 1))) * ((int32_t)sensor->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)sensor->dig_T1)) * ((adc_T >> 4) - ((int32_t)sensor->dig_T1))) >> 12) * ((int32_t)sensor->dig_T3)) >> 14;
    t_fine = var1 + var2;
    *temperature = (float)((t_fine * 5 + 128) >> 8) / 100.0f;
    
    // Cálculo da pressão compensada
    int64_t var3, var4, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)sensor->dig_P6;
    var2 = var2 + ((var1 * (int64_t)sensor->dig_P5) << 17);
    var2 = var2 + (((int64_t)sensor->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)sensor->dig_P3) >> 8) + ((var1 * (int64_t)sensor->dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)sensor->dig_P1) >> 33;
    
    if(var1 == 0) {
        return false; // Evita divisão por zero
    }
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)sensor->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)sensor->dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)sensor->dig_P7) << 4);
    *pressure = (float)p / 256.0f; // Resultado em Pa
    
    // Cálculo da altitude (opcional)
    if(altitude != NULL) {
        *altitude = 44330.0f * (1.0f - powf(*pressure / sensor->sea_level_pressure, 0.1903f));
    }
    
    return true;
}

void bmp280_set_sea_level_pressure(bmp280_t *sensor, float pressure) {
    sensor->sea_level_pressure = pressure;
}