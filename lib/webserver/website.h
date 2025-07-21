#ifndef WEB_SITE_H
#define WEB_SITE_H

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Configurações de WiFi
#define WIFI_SSID "S@NTOS$28"
#define WIFI_PASSWORD "p&1xe$82"

// Estrutura para armazenar dados dos sensores
typedef struct {
    float aht_temp;
    float aht_humidity;
    float bmp_pressure;
    float bmp_temp;
    float bmp_altitude;
} sensor_data_t;

// Estrutura para configurações
typedef struct {
    float temp_min;
    float temp_max;
    float humidity_min;
    float humidity_max;
    float sea_level_pressure;
} config_settings_t;

// Estrutura para o estado HTTP
typedef struct {
    char response[5300];  // Buffer para a resposta HTTP
    size_t len;           // Tamanho total da resposta
    size_t sent;          // Quantidade já enviada
} http_state_t;

// Protótipos de funções
void init_web_server(void);
void update_sensor_data(sensor_data_t new_data);
void update_config_settings(config_settings_t new_config);

#endif // WEB_SITE_H