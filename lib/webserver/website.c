#include "lib/webserver/website.h"
#include "lib/webserver/html.h" // Arquivo com o HTML do site

// Variáveis globais
static sensor_data_t current_sensor_data = {0};
static config_settings_t current_config = {
    .temp_min = 10.0,
    .temp_max = 30.0,
    .humidity_min = 30.0,
    .humidity_max = 70.0,
    .sea_level_pressure = 101325.0
};

// Callback para quando dados são enviados
static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    http_state_t *hs = (http_state_t *)arg;
    hs->sent += len;
    
    if (hs->sent >= hs->len) {
        tcp_close(tpcb);
        free(hs);
    }
    return ERR_OK;
}

// Callback para requisições HTTP
static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = (char *)p->payload;
    http_state_t *hs = malloc(sizeof(http_state_t));
    if (!hs) {
        pbuf_free(p);
        tcp_close(tpcb);
        tcp_recved(tpcb, p->tot_len);
        return ERR_MEM;
    }
    hs->sent = 0;

    // Requisição para dados dos sensores
    if (strstr(req, "GET /api/sensor-data")) {
        char json_payload[256];
        int json_len = snprintf(json_payload, sizeof(json_payload),
            "{\"aht\":{\"temp\":%.1f,\"humidity\":%.1f},"
            "\"bmp\":{\"pressure\":%.1f,\"temp\":%.1f,\"altitude\":%.1f},"
            "\"average\":{\"temp\":%.1f}}\r\n",
            current_sensor_data.aht_temp,
            current_sensor_data.aht_humidity,
            current_sensor_data.bmp_pressure,
            current_sensor_data.bmp_temp,
            current_sensor_data.bmp_altitude,
            (current_sensor_data.aht_temp + current_sensor_data.bmp_temp) / 2);

        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            json_len, json_payload);
    }
    // Requisição para atualizar configurações
    else if (strstr(req, "POST /api/config")) {
        // Simples exemplo - em produção, deveria parsear o corpo JSON
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Content-Length: 2\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{}");
    }
    // Página HTML principal
    else {
        hs->len = snprintf(hs->response, sizeof(hs->response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            (int)strlen(html), html);
    }

    tcp_arg(tpcb, hs);
    tcp_sent(tpcb, http_sent);
    tcp_write(tpcb, hs->response, hs->len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    pbuf_free(p);
    return ERR_OK;
}

// Callback para novas conexões
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

// Inicia o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB TCP\n");
        return;
    }
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

// Inicializa o servidor web
void init_web_server(void) {
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar cyw43\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, 
                                          CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro ao conectar ao WiFi\n");
        return;
    }

    // Obtém e imprime o endereço IP
    uint8_t *ip = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("Conectado! IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

    start_http_server();
}

// Atualiza os dados dos sensores
void update_sensor_data(sensor_data_t new_data) {
    current_sensor_data = new_data;
}

// Atualiza as configurações
void update_config_settings(config_settings_t new_config) {
    current_config = new_config;
}