// Arquivo: lib/webserver/website.c
// Versão com correção do bug de limites

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "website.h"

// --- Estrutura para manter o estado de cada conexão ---
// --- Estrutura para manter o estado de cada conexão ---
typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    sensor_data_t *sensor_data;
    bool complete;
    char response[4096];
    char request[1500];     // Buffer para acumular a requisição
    uint16_t request_len;   // Tamanho atual da requisição acumulada
} TCP_SERVER_T;

// --- Conteúdo do Site ---
const char INDEX_HTML[] =
"<!DOCTYPE html>"
"<html lang=\"pt-br\">"
"<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>Estação Meteorológica IoT</title>"
    "<link rel=\"stylesheet\" href=\"styles.css\">"
"</head>"
"<body>"
    "<div class=\"container\">"
        "<header>"
            "<h1>Estação Meteorológica IoT com Pico W</h1>"
        "</header>"
        ""
        "<div id=\"alert-message\" class=\"alert-container\" style=\"display:none;\"></div>"
        "<main>"
            "<section class=\"data-cards\">"
                "<div class=\"card\">"
                    "<h2>Temperatura</h2>"
                    "<p class=\"sensor-value\" id=\"temp_med\">--</p>"
                    "<div class=\"sub-data\">"
                        "<p>AHT20: <span id=\"temp_aht\">--</span></p>"
                        "<p>BMP280: <span id=\"temp_bmp\">--</span></p>"
                    "</div>"
                "</div>"
                "<div class=\"card\">"
                    "<h2>Umidade</h2>"
                    "<p class=\"sensor-value\" id=\"humidity\">--</p>"
                "</div>"
                "<div class=\"card\">"
                    "<h2>Pressão</h2>"
                    "<p class=\"sensor-value\" id=\"pressure\">--</p>"
                "</div>"
                "<div class=\"card\">"
                    "<h2>Altitude</h2>"
                    "<p class=\"sensor-value\" id=\"altitude\">--</p>"
                "</div>"
            "</section>"
            "<section class=\"charts\">"
                "<div class=\"chart-container\">"
                    "<h3>Histórico de Temperatura (°C)</h3>"
                    "<canvas id=\"tempChart\"></canvas>"
                "</div>"
                "<div class=\"chart-container\">"
                    "<h3>Histórico de Umidade (%)</h3>"
                    "<canvas id=\"humidityChart\"></canvas>"
                "</div>"
                ""
                "<div class=\"chart-container\">"
                    "<h3>Histórico de Pressão (hPa)</h3>"
                    "<canvas id=\"pressureChart\"></canvas>"
                "</div>"
            "</section>"
            "<section class=\"settings\">"
                "<h2>Configurações de Limites</h2>"
                "<form id=\"settingsForm\">"
                    "<div class=\"form-group\">"
                        "<label for=\"temp_min\">Temp. Mínima (°C)</label>"
                        "<input type=\"number\" id=\"temp_min\" name=\"temp_min_limit\" step=\"0.1\">"
                    "</div>"
                    "<div class=\"form-group\">"
                        "<label for=\"temp_max\">Temp. Máxima (°C)</label>"
                        "<input type=\"number\" id=\"temp_max\" name=\"temp_max_limit\" step=\"0.1\">"
                    "</div>"
                    "<div class=\"form-group\">"
                        "<label for=\"humidity_min\">Umidade Mínima (%)</label>"
                        "<input type=\"number\" id=\"humidity_min\" name=\"humidity_min_limit\" step=\"0.1\">"
                    "</div>"
                    "<div class=\"form-group\">"
                        "<label for=\"humidity_max\">Umidade Máxima (%)</label>"
                        "<input type=\"number\" id=\"humidity_max\" name=\"humidity_max_limit\" step=\"0.1\">"
                    "</div>"
                    "<button type=\"submit\">Salvar Limites</button>"
                "</form>"
                "<p id=\"form-status\"></p>"
            "</section>"
        "</main>"
        "<footer>"
            "<p>Desenvolvido com Raspberry Pi Pico W</p>"
        "</footer>"
    "</div>"
    "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>"
    "<script src=\"scripts.js\"></script>"
"</body>"
"</html>";

const char STYLES_CSS[] =
":root{--primary-color:#007bff;--secondary-color:#6c757d;--background-color:#f8f9fa;--card-bg-color:#fff;--text-color:#333;--border-color:#dee2e6;--shadow-color:rgba(0,0,0,.1);--alert-color:#dc3545}"
"body{font-family:-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,sans-serif;background-color:var(--background-color);color:var(--text-color);margin:0;padding:20px;line-height:1.6}"
".container{max-width:1200px;margin:0 auto;padding:0 15px}"
"header{text-align:center;margin-bottom:2rem;color:var(--primary-color)}"
".alert-container{background-color:var(--alert-color);color:#fff;padding:15px;margin-bottom:1.5rem;border-radius:8px;text-align:center;font-weight:700}"
".data-cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:20px;margin-bottom:2rem}"
".card{background-color:var(--card-bg-color);padding:20px;border-radius:8px;box-shadow:0 2px 4px var(--shadow-color);text-align:center;transition:transform .2s}"
".card:hover{transform:translateY(-5px)}"
".card h2{margin-top:0;color:var(--secondary-color);font-size:1.2rem}"
".sensor-value{font-size:2.5rem;font-weight:700;color:var(--primary-color);margin:10px 0}"
".sub-data{font-size:.9rem;color:var(--secondary-color)}"
".charts{display:grid;grid-template-columns:repeat(auto-fit,minmax(400px,1fr));gap:20px;margin-bottom:2rem}"
".chart-container{background-color:var(--card-bg-color);padding:20px;border-radius:8px;box-shadow:0 2px 4px var(--shadow-color)}"
".settings{background-color:var(--card-bg-color);padding:20px 30px;border-radius:8px;box-shadow:0 2px 4px var(--shadow-color)}"
".settings h2{text-align:center;margin-top:0}"
"#settingsForm{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:20px;align-items:center}"
".form-group{display:flex;flex-direction:column}"
"#settingsForm label{margin-bottom:5px;font-weight:700}"
"#settingsForm input{padding:10px;border:1px solid var(--border-color);border-radius:4px;font-size:1rem}"
"#settingsForm button{background-color:var(--primary-color);color:#fff;padding:12px;border:none;border-radius:4px;cursor:pointer;font-size:1rem;grid-column:1/-1;margin-top:10px;transition:background-color .2s}"
"#settingsForm button:hover{background-color:#0056b3}"
"#form-status{text-align:center;margin-top:15px;font-weight:700;min-height:1.2em}"
"footer{text-align:center;margin-top:2rem;color:var(--secondary-color)}";

// JavaScript CORRIGIDO com debug
const char SCRIPTS_JS[] =
"document.addEventListener('DOMContentLoaded',function(){'use strict';"
"const MAX_DATA_POINTS=30;"
"const updateElementText=(id,text,unit='')=>{const el=document.getElementById(id);if(el){if(typeof text==='number'&&!isNaN(text)){el.textContent=text.toFixed(2)+unit}else{el.textContent=text}}};"
"const createChart=(ctx,label,color)=>{return new Chart(ctx,{type:'line',data:{labels:[],datasets:[{label:label,data:[],borderColor:color,backgroundColor:color.replace('1)','0.2)'),borderWidth:2,fill:true,tension:.3}]},options:{scales:{x:{ticks:{display:false}},y:{beginAtZero:false}},animation:{duration:400}}})};"
"const tempChart=createChart(document.getElementById('tempChart').getContext('2d'),'Temperatura','rgba(0,123,255,1)');"
"const humidityChart=createChart(document.getElementById('humidityChart').getContext('2d'),'Umidade','rgba(220,53,69,1)');"
"const pressureChart=createChart(document.getElementById('pressureChart').getContext('2d'),'Pressão','rgba(40,167,69,1)');"
"const updateChart=(chart,value)=>{const now=new Date();const label=`${now.getHours()}:${now.getMinutes().toString().padStart(2,'0')}:${now.getSeconds().toString().padStart(2,'0')}`;chart.data.labels.push(label);chart.data.datasets[0].data.push(value);if(chart.data.labels.length>MAX_DATA_POINTS){chart.data.labels.shift();chart.data.datasets[0].data.shift()}chart.update()};"
"const fetchData=async()=>{try{const response=await fetch('/data.json');if(!response.ok)throw new Error(`Erro HTTP: ${response.status}`);const data=await response.json();updateElementText('temp_med',data.temp_med,'°C');updateElementText('temp_aht',data.temp_aht,'°C');updateElementText('temp_bmp',data.temp_bmp,'°C');updateElementText('humidity',data.humidity,'%');updateElementText('pressure',data.pressure/100,' hPa');updateElementText('altitude',data.altitude,' m');updateChart(tempChart,data.temp_med);updateChart(humidityChart,data.humidity);updateChart(pressureChart,data.pressure/100);"
"const alertEl=document.getElementById('alert-message');if(data.outside_limits){alertEl.textContent='Atenção: Uma ou mais medições estão fora dos limites definidos!';alertEl.style.display='block'}else{alertEl.style.display='none'}}catch(error){console.error('Erro ao buscar dados:',error)}};"
"const settingsForm=document.getElementById('settingsForm');"
"const formStatus=document.getElementById('form-status');"
"const loadCurrentSettings=async()=>{try{const response=await fetch('/data.json');if(!response.ok)return;const data=await response.json();document.getElementById('temp_min').value=data.temp_min_limit.toFixed(1);document.getElementById('temp_max').value=data.temp_max_limit.toFixed(1);document.getElementById('humidity_min').value=data.humidity_min_limit.toFixed(1);document.getElementById('humidity_max').value=data.humidity_max_limit.toFixed(1)}catch(error){console.error('Erro ao carregar configs:',error)}};"
"settingsForm.addEventListener('submit',async(event)=>{event.preventDefault();const formData=new FormData(settingsForm);const data=Object.fromEntries(formData.entries());console.log('DEBUG: Dados sendo enviados:',data);formStatus.textContent='Salvando...';formStatus.style.color='#333';try{const response=await fetch('/settings',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)});if(response.ok){formStatus.textContent='Limites salvos com sucesso!';formStatus.style.color='green'}else{throw new Error(`Falha ao salvar. Status: ${response.status}`)}}catch(error){console.error('Erro ao enviar form:',error);formStatus.textContent='Erro ao salvar. Tente novamente.';formStatus.style.color='red'}setTimeout(()=>{formStatus.textContent=''},3000)});"
"fetchData();loadCurrentSettings();setInterval(fetchData,2000);"
"});";

// --- Funções do servidor TCP ---
static err_t tcp_server_close(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (state && state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_poll(state->server_pcb, NULL, 0);
        tcp_sent(state->server_pcb, NULL);
        tcp_recv(state->server_pcb, NULL);
        tcp_err(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
    }
    if (state) {
        free(state);
    }
    return ERR_OK;
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (state->complete) {
        return tcp_server_close(state);
    }
    return ERR_OK;
}

static void send_http_response(void *arg, const char *content_type, const char *body) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    state->complete = true;

    int len = snprintf(state->response, sizeof(state->response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n%s",
        content_type, (int)strlen(body), body);
    
    cyw43_arch_lwip_begin();
    err_t err = tcp_write(state->server_pcb, state->response, len, TCP_WRITE_FLAG_COPY);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        printf("Erro ao escrever para o buffer TCP: %d\n", err);
        tcp_server_close(state);
    }
}

// FUNÇÃO CORRIGIDA para processar requisições HTTP
// FUNÇÃO CORRIGIDA para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (!p) {
        return tcp_server_close(state);
    }
    
    tcp_recved(tpcb, p->tot_len);

    // Itera sobre a cadeia de pbufs para anexar os dados ao buffer de estado
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        // Garante que não vamos estourar o buffer
        if (state->request_len + q->len < sizeof(state->request)) {
            memcpy(state->request + state->request_len, q->payload, q->len);
            state->request_len += q->len;
        }
    }
    // Adiciona um terminador nulo para segurança com funções de string
    state->request[state->request_len] = '\0';

    pbuf_free(p);

    // Agora, processamos o buffer acumulado (state->request)
    // Verificamos se a requisição está completa o suficiente para ser processada
    // (Ex: contém o fim dos headers para GET, ou o corpo completo para POST)

    // Roteamento das requisições
    if (strstr(state->request, "GET /scripts.js")) {
        send_http_response(state, "application/javascript", SCRIPTS_JS);
    } else if (strstr(state->request, "GET /styles.css")) {
        send_http_response(state, "text/css", STYLES_CSS);
    } else if (strstr(state->request, "GET /data.json")) {
        bool outside_limits = false;
        if (state->sensor_data->temp_med < state->sensor_data->temp_min_limit ||
            state->sensor_data->temp_med > state->sensor_data->temp_max_limit ||
            state->sensor_data->humidity < state->sensor_data->humidity_min_limit ||
            state->sensor_data->humidity > state->sensor_data->humidity_max_limit) {
            outside_limits = true;
        }

        char json_payload[512];
        snprintf(json_payload, sizeof(json_payload),
            "{\"temp_aht\": %.2f, \"temp_bmp\": %.2f, \"temp_med\": %.2f, "
            "\"humidity\": %.2f, \"pressure\": %.2f, \"altitude\": %.2f, "
            "\"temp_min_limit\": %.1f, \"temp_max_limit\": %.1f, "
            "\"humidity_min_limit\": %.1f, \"humidity_max_limit\": %.1f, "
            "\"outside_limits\": %s}",
            state->sensor_data->temp_aht, state->sensor_data->temp_bmp, state->sensor_data->temp_med,
            state->sensor_data->humidity, state->sensor_data->pressure, state->sensor_data->altitude,
            state->sensor_data->temp_min_limit, state->sensor_data->temp_max_limit,
            state->sensor_data->humidity_min_limit, state->sensor_data->humidity_max_limit,
            outside_limits ? "true" : "false"
        );
        send_http_response(state, "application/json", json_payload);

    } else if (strstr(state->request, "POST /settings")) {
        // SEÇÃO CORRIGIDA - Processamento dos limites
        char *json_start = strstr(state->request, "\r\n\r\n");
        if (json_start) {
            json_start += 4; // Pula o \r\n\r\n
            
            // Debug: mostra o JSON recebido
            printf("DEBUG: JSON recebido: %.200s\n", json_start);

            // Função auxiliar para extrair valores JSON
            void extract_float_from_json(const char* json, const char* key, float* dest) {
                char search_pattern[64];
                snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
                char* key_ptr = strstr(json, search_pattern);
                
                if (key_ptr) {
                    key_ptr += strlen(search_pattern);
                    while (*key_ptr == ' ' || *key_ptr == '"') key_ptr++; // Pula espaços e aspas
                    *dest = atof(key_ptr);
                    printf("DEBUG: Extraído %s = %.2f\n", key, *dest);
                } else {
                    printf("DEBUG: Chave '%s' não encontrada no JSON\n", key);
                }
            }
            
            // Extrai os valores dos limites
            extract_float_from_json(json_start, "temp_min_limit", &state->sensor_data->temp_min_limit);
            extract_float_from_json(json_start, "temp_max_limit", &state->sensor_data->temp_max_limit);
            extract_float_from_json(json_start, "humidity_min_limit", &state->sensor_data->humidity_min_limit);
            extract_float_from_json(json_start, "humidity_max_limit", &state->sensor_data->humidity_max_limit);
            
            printf("NOVOS LIMITES APLICADOS: T_min=%.1f, T_max=%.1f, H_min=%.1f, H_max=%.1f\n",
                state->sensor_data->temp_min_limit, state->sensor_data->temp_max_limit,
                state->sensor_data->humidity_min_limit, state->sensor_data->humidity_max_limit);
        } else {
            printf("DEBUG: Não foi possível encontrar o corpo do POST\n");
        }
        send_http_response(state, "text/plain", "OK");
        
    } else if (strstr(state->request, "GET /")) { // Roteamento explícito para a página principal
        send_http_response(state, "text/html", INDEX_HTML);
    }

    // Apenas fecha a conexão se a resposta foi marcada como completa (pela send_http_response)
    // O fechamento real ocorrerá na callback tcp_server_sent
    if (state->complete) {
        // Não faz nada aqui, espera o sent callback
    }

    return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    if (newpcb == NULL) {
        return ERR_MEM;
    }

    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        tcp_close(newpcb);
        return ERR_MEM;
    }
    
    state->server_pcb = newpcb;
    state->sensor_data = (sensor_data_t*)arg;

    tcp_arg(newpcb, state);
    tcp_recv(newpcb, tcp_server_recv);
    tcp_sent(newpcb, tcp_server_sent);
    tcp_err(newpcb, (tcp_err_fn)tcp_server_close);

    return ERR_OK;
}

void webserver_init(sensor_data_t *data) {
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        printf("Erro ao criar PCB TCP\n");
        return;
    }

    err_t err = tcp_bind(pcb, IP_ADDR_ANY, 80);
    if (err != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80 (erro %d)\n", err);
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_arg(pcb, data);
    tcp_accept(pcb, tcp_server_accept);
    
    printf("Servidor HTTP com debug de limites iniciado na porta 80\n");
}