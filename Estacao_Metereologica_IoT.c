#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "lwip/netif.h"
#include "lib/aht20.h"
#include "lib/bmp280.h"
#include "lib/webserver/website.h"
#include "lib/buzzer.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include <math.h>
#include <string.h>

// Configuração do Hardware
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define I2C_PORT i2c0
#define I2C_FREQ 100000  // 100kHz
#define READ_INTERVAL_MS 2000
#define GREEN_LED 11
#define RED_LED 13
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define DISP_ADDRESS 0X3C
#define BUTTON_A 5
#define BUTTON_B 6
#define DEBOUNCE_TIME 200

// Configurações de WiFi
#define WIFI_SSID "SUA_REDE"
#define WIFI_PASSWORD "SUA_SENHA"

#define STANDARD_SEA_LEVEL_PRESSURE 101325.0

volatile uint32_t last_time = 0;
bool page_mode = false;
bool alarm_mode = true;

// Dados compartilhados com o servidor web
sensor_data_t sensor_data = {
    .temp_min_limit = 10.0f,
    .temp_max_limit = 35.0f,
    .humidity_min_limit = 30.0f,
    .humidity_max_limit = 70.0f
};

bool check_limits(sensor_data_t *data){
    return data->temp_med < data->temp_min_limit || data->temp_med > data->temp_max_limit || data->humidity > data->humidity_max_limit || data->humidity < data->humidity_min_limit;
}

// Função para obter e imprimir o endereço IP
void print_ip_address() {
    struct netif *netif = netif_default;
    if (netif != NULL && netif_is_up(netif)) {
        printf("Endereço IP: %s\n", ip4addr_ntoa(&netif->ip_addr));
        printf("Máscara de Rede: %s\n", ip4addr_ntoa(&netif->netmask));
        printf("Gateway: %s\n", ip4addr_ntoa(&netif->gw));
    } else {
        printf("Interface de rede não disponível\n");
    }
}

double calculate_altitude(double pressure)
{
    return 44330.0 * (1.0 - pow(pressure / STANDARD_SEA_LEVEL_PRESSURE, 0.1903));
}
void buttons_callback(uint gpio, uint32_t events){
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time-last_time>=DEBOUNCE_TIME){
        if (gpio == BUTTON_A){
            page_mode = !page_mode;
        } else if(gpio == BUTTON_B){
            alarm_mode = !alarm_mode;
            printf("Botão pressionado \n Alarm Mode %d", alarm_mode);
        }
        
        last_time == current_time;
    }
}
void core1_entry(){
    // Configuração do Buzzer
    init_buzzer_pwm(BUZZER_A);
    // Configuração dos Leds
    gpio_init(GREEN_LED);
    gpio_init(RED_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_put(GREEN_LED, false);
    gpio_put(RED_LED, false);
    // Configuração do display OLED
    i2c_init(I2C_PORT_DISP, 400 * 1000);
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISP);
    gpio_pull_up(I2C_SCL_DISP);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISP_ADDRESS, I2C_PORT_DISP);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    bool color = true;
    // Delay para aguardar a primeira medição e não ativar o alarme desnecessariamente
    sleep_ms(5000);
    while(true){
        char str_temp[16], str_press[16], str_hum[16], str_alt[16];
        char temp_print[16] = "Temp: ", press_print[16] = "Pres: ", hum_print[16] = "Umid: ", alt_print[16] = "Alti: ";
        char str_temp_max[16], str_temp_min[16], str_hum_max[16], str_hum_min[16];
        char temp_max_print[16]="Temp Max:", temp_min_print[16]="Temp Min", hum_max_print[16]="Umi Max:", hum_min_print[16]="Umi Min:";
        if (check_limits(&sensor_data)){
            if (alarm_mode){
                play_alarm_critic();
            }
            gpio_put(GREEN_LED, false);
            gpio_put(RED_LED, true);
        }else{
            gpio_put(GREEN_LED, true);
            gpio_put(RED_LED, false);
        }
        // Formatação da string da Pagina principal do display
        sprintf(str_temp, "%1.2f", sensor_data.temp_med);
        sprintf(str_press, "%1.2f", sensor_data.pressure/100);
        sprintf(str_hum, "%1.2f", sensor_data.humidity);
        sprintf(str_alt, "%1.2f", sensor_data.altitude);
        strcat(temp_print, str_temp);
        strcat(press_print, str_press);
        strcat(hum_print, str_hum);
        strcat(alt_print, str_alt);
        strcat(temp_print, "C");
        strcat(press_print, "hPa");
        strcat(hum_print, "%%");
        strcat(alt_print, "m");
        // Formatação da string da pagina secundaria do display
        sprintf(str_temp_max, "%1.2f", sensor_data.temp_max_limit);
        sprintf(str_temp_min, "%1.2f", sensor_data.temp_min_limit);
        sprintf(str_hum_max, "%1.2f", sensor_data.humidity_max_limit);
        sprintf(str_hum_min, "%1.2f", sensor_data.humidity_min_limit);
        strcat(temp_max_print, str_temp_max);
        strcat(temp_min_print, str_temp_min);
        strcat(hum_max_print, str_hum_max);
        strcat(hum_min_print, str_hum_min);
        strcat(temp_max_print, "C");
        strcat(temp_min_print, "C");
        strcat(hum_max_print, "%%");
        strcat(hum_min_print, "%%");

        ssd1306_fill(&ssd, !color);
        if (page_mode){
            ssd1306_draw_string(&ssd, temp_max_print, 1, 5);
            ssd1306_draw_string(&ssd, temp_min_print, 1, 20);
            ssd1306_draw_string(&ssd, hum_max_print, 1, 35);
            ssd1306_draw_string(&ssd, hum_min_print, 1, 50);
        } else {
            ssd1306_draw_string(&ssd, temp_print, 1, 5);
            ssd1306_draw_string(&ssd, press_print, 1, 20);
            ssd1306_draw_string(&ssd, hum_print, 1, 35);
            ssd1306_draw_string(&ssd, alt_print, 1, 50);
        }
        ssd1306_send_data(&ssd);
        sleep_ms(500);
    }
}

int main() {
    // Configuração dos Botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    // Interrupção dos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &buttons_callback);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
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
    struct bmp280_calib_param bmp_params;
    
    // Inicialização AHT20
    if(!aht20_init(&aht, I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ)) {
        printf("Erro: AHT20 não encontrado no endereço 0x38\n");
        return 1;
    } else {
        printf("AHT20 OK (0x38)\n");
    }

    // Inicialização BMP280
    printf("Inicializando BMP280...\n");
    
    // Reset do BMP280
    bmp280_reset(I2C_PORT);
    sleep_ms(100);  // Aguarda reset
    
    // Inicialização do BMP280
    bmp280_init(I2C_PORT);
    sleep_ms(100);
    
    // Carrega parâmetros de calibração
    bmp280_get_calib_params(I2C_PORT, &bmp_params);
    printf("BMP280 OK (0x76)\n");
    
    
    // Inicialização WiFi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar WiFi\n");
        return 1;
    }
    
    // Conecta ao WiFi
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao WiFi %s...\n", WIFI_SSID);
    
    int retry = 0;
    while (retry < 3) {
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
            printf("Conectado ao WiFi com sucesso!\n");
            
            // Aguarda DHCP
            for (int i = 0; i < 10; i++) {
                if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) {
                    print_ip_address();
                    break;
                }
                sleep_ms(1000);
                printf("Aguardando atribuição de IP... (%d/10)\n", i+1);
            }
            break;
        }
        
        printf("Falha na conexão (tentativa %d/3)\n", retry+1);
        retry++;
        sleep_ms(3000);
    }

    if (retry >= 3) {
        printf("Não foi possível conectar ao WiFi\n");
        return 1;
    }
    
    printf("\n=== Iniciando o WebServer ===\n\n");
    webserver_init(&sensor_data);
    printf("\n=== Iniciando Leituras ===\n\n");
    multicore_launch_core1(core1_entry);
    while(1) {
        cyw43_arch_poll();
        
        // Leitura AHT20 (Temperatura e Umidade)
        if(!aht20_read(&aht, &sensor_data.temp_aht, &sensor_data.humidity)) {
            printf("Erro na leitura do AHT20\n");
        } else {
            printf("AHT20 - Temp: %.2f°C | Umidade: %.2f%%\n", sensor_data.temp_aht, sensor_data.humidity);
        }

        // Leitura BMP280 (Temperatura e Pressão)
        int32_t raw_temp, raw_pressure;
        bmp280_read_raw(I2C_PORT, &raw_temp, &raw_pressure);
        
        if (raw_temp != 0 && raw_pressure != 0) {
            // Conversão dos valores usando os parâmetros de calibração
            int32_t temp_converted = bmp280_convert_temp(raw_temp, &bmp_params);
            int32_t pressure_converted = bmp280_convert_pressure(raw_pressure, raw_temp, &bmp_params);
            
            // Converte para valores em float
            sensor_data.temp_bmp = temp_converted / 100.0f;  // Temperatura em °C
            sensor_data.pressure = pressure_converted;       // Pressão em Pa
            
            // Calcula altitude
            if (sensor_data.pressure > 0) {
                double pressure_ratio = sensor_data.pressure / STANDARD_SEA_LEVEL_PRESSURE;
                sensor_data.altitude = 44330.0f * (1.0f - powf(pressure_ratio, 0.1903f));
            } else {
                sensor_data.altitude = 0.0f;
            }
            
            printf("BMP280 - Temp: %.2f°C | Pressão: %.2f hPa | Altitude: %.2f m\n", 
                   sensor_data.temp_bmp, sensor_data.pressure/100.0f, sensor_data.altitude);
        } else {
            printf("Erro na leitura do BMP280\n");
        }
        
        // Cálculo da temperatura média
        if (sensor_data.temp_aht > 0 && sensor_data.temp_bmp > 0) {
            sensor_data.temp_med = (sensor_data.temp_aht + sensor_data.temp_bmp) / 2;
            printf("Temperatura Média: %.2f°C\n", sensor_data.temp_med);
        }
        printf("Limites - Temp: %.2f°C (min), %.2f°C (max) | Umidade: %.2f%% (min), %.2f%% (max)\n",
               sensor_data.temp_min_limit, sensor_data.temp_max_limit,
               sensor_data.humidity_min_limit, sensor_data.humidity_max_limit);
        if (sensor_data.temp_med < sensor_data.temp_min_limit || sensor_data.temp_med > sensor_data.temp_max_limit){
            play_alarm_critic();
        }
        printf("----------------------------\n");
        
        cyw43_arch_poll();
        sleep_ms(READ_INTERVAL_MS);
    }
}
