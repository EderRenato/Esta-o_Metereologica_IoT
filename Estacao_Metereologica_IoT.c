#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lib/aht20.h"
#include "lib/bmp280.h"
#include "lib/webserver/website.h"

// Configuração do Hardware
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_FREQ 100000  // 100kHz
#define READ_INTERVAL_MS 2000

#define STANDARD_SEA_LEVEL_PRESSURE 101325.0

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main() {

     // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim do trecho para modo BOOTSEL com botão B
    // Inicialização básica
    stdio_init_all();
    sleep_ms(2000);  // Espera para inicialização serial
    
    printf("\n=== Inicializando Sistema de Sensores ===\n");

    // Configuração I2C
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Estruturas dos sensores
    aht20_t aht;
    bmp280_t bmp;
    sensor_data_t medicoes;
    
    // Inicialização AHT20
    if(!aht20_init(&aht, I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ)) {
        printf("Erro: AHT20 não encontrado no endereço 0x%02X\n", AHT20_I2C_ADDR);
        return 1;
    } else {
        printf("AHT20 OK (0x38)\n");
    }

    // Inicialização BMP280
    if(!bmp280_init(&bmp, I2C_PORT, BMP280_I2C_ADDR)) {
        printf("Erro: BMP280 não encontrado no endereço 0x%02X\n", BMP280_I2C_ADDR);
        return 1;
    } else if(!bmp280_config(&bmp, BMP280_OS_4X, BMP280_OS_4X, 
                           BMP280_NORMAL_MODE, BMP280_FILTER_4, BMP280_STANDBY_250)) {
        printf("Erro: Configuração do BMP280 falhou\n");
        return 1;
    } else {
        printf("BMP280 OK (0x76)\n");
    }

    bmp280_set_sea_level_pressure(&bmp, STANDARD_SEA_LEVEL_PRESSURE); // Em Pascals
    
    printf("\n=== Iniciando o WebServer ===\n\n");
    init_web_server();
    printf("\n=== Iniciando Leituras ===\n\n");

    while(1) {
        // Leitura AHT20 (Temperatura e Umidade)
        float temp_aht = 0, humidity = 0;
        if(!aht20_read(&aht, &temp_aht, &humidity)) {
            printf("Erro na leitura do AHT20\n");
        } else {
            printf("AHT20 - Temp: %.2fC | Umidade: %.2f%%\n", temp_aht, humidity);
        }

        // Leitura BMP280 (Temperatura, Pressão e Altitude)
        float temp_bmp = 0, pressure = 0, altitude = 0;
        if(!bmp280_read(&bmp, &temp_bmp, &pressure, &altitude)) {
            printf("Erro na leitura do BMP280\n");
        } else {
            printf("BMP280 - Temp: %.2fC | Pressão: %.2fhPa | Altitude: %.2fm\n", 
                  temp_bmp, pressure/100.0f, altitude);
        }
        
        // Cálculo da temperatura média
        float temp_med = (temp_aht + temp_bmp) / 2;
        printf("Temperatura Média medida: %.2fC\n", temp_med);
        printf("----------------------------\n");
        
        // Atualiza os dados para o servidor web
        medicoes.aht_temp = temp_aht;
        medicoes.aht_humidity = humidity;
        medicoes.bmp_pressure = pressure / 100.0f; // Convertendo para hPa
        medicoes.bmp_temp = temp_bmp;
        medicoes.bmp_altitude = altitude;
        
        update_sensor_data(medicoes);
        sleep_ms(READ_INTERVAL_MS);
    }
}