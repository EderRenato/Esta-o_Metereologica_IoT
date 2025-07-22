#ifndef WEBSITE_H
#define WEBSITE_H

#include "pico/stdlib.h"

// Estrutura para compartilhar dados entre o loop principal e o servidor web.
typedef struct {
    float temp_aht;
    float temp_bmp;
    float temp_med;
    float humidity;
    float pressure;
    float altitude;

    float temp_min_limit;
    float temp_max_limit;
    float humidity_min_limit;
    float humidity_max_limit;
} sensor_data_t;

// Declaração da função de inicialização do servidor web
void webserver_init(sensor_data_t *data);

#endif // WEBSITE_H