# Estação Meteorológica IoT com Raspberry Pi Pico W

Este projeto é uma estação meteorológica completa construída com um Raspberry Pi Pico W. Ele monitoriza a temperatura, humidade, pressão e altitude, exibindo os dados num display OLED e numa interface web moderna e interativa. Inclui um sistema de alarme configurável para notificar quando as medições ultrapassam os limites definidos.

## ✨ Funcionalidades

  * **Monitorização de Sensores**:
      * Leitura de temperatura e humidade com o sensor AHT20.
      * Leitura de temperatura e pressão barométrica com o sensor BMP280.
      * Cálculo da temperatura média a partir de ambos os sensores.
      * Cálculo de altitude com base na pressão.
  * **Display Local**:
      * Exibição dos dados em tempo real num display OLED SSD1306.
      * Interface no display com duas páginas: dados atuais e limites de alarme, alternáveis através de um botão.
  * **Interface Web Completa**:
      * Servidor web hospedado no Pico W para visualização remota dos dados.
      * Página web com design responsivo que exibe os valores atuais dos sensores.
      * Gráficos históricos para temperatura, humidade e pressão, atualizados dinamicamente.
      * Formulário na interface web para configurar os limites mínimo e máximo de temperatura e humidade.
  * **Sistema de Alarme**:
      * Alarmes visuais (LEDs vermelho/verde) e sonoros (buzzer) quando os valores saem dos limites configurados.
      * Possibilidade de ativar/desativar o alarme sonoro através de um botão físico.
  * **Conectividade**:
      * Conexão à rede Wi-Fi para acesso à interface web.

## 🛠️ Hardware Necessário

  * Raspberry Pi Pico W
  * Sensor de Temperatura e Humidade AHT20
  * Sensor de Pressão Barométrica BMP280
  * Display OLED I2C SSD1306 (128x64)
  * Buzzer Ativo
  * 1x LED Verde
  * 1x LED Vermelho
  * 2x Botões de pressão (Push Buttons)
  * Resistores apropriados para os LEDs
  * Protoboard e fios de ligação

## ⚙️ Configuração e Instalação

### 1\. Ambiente de Desenvolvimento

Para compilar e carregar o projeto, é necessário ter o ambiente de desenvolvimento para o Raspberry Pi Pico configurado.

  * **Instale o Pico SDK**, o compilador `arm-none-eabi-gcc` e o `CMake`. Siga as instruções oficiais em [Getting started with Raspberry Pi Pico](https://www.google.com/search?q=https://projects.raspberrypi.org/en/projects/getting-started-with-the-raspberry-pi-pico).
  * **Visual Studio Code (Recomendado)**: O projeto está configurado para ser utilizado com o VS Code. Instale as extensões recomendadas no ficheiro `.vscode/extensions.json` para uma melhor experiência.

### 2\. Configurar o Projeto

1.  **Clone o repositório**:

    ```bash
    git clone <URL_DO_REPOSITORIO>
    cd <NOME_DO_DIRETORIO>
    ```

2.  **Credenciais Wi-Fi**: Abra o ficheiro `Estacao_Metereologica_IoT.c` e altere as seguintes linhas com as credenciais da sua rede Wi-Fi:

    ```c
    #define WIFI_SSID "SEU_SSID"
    #define WIFI_PASSWORD "SUA_SENHA"
    ```

### 3\. Compilar e Carregar

O projeto utiliza `CMake` e `Ninja` para a compilação. Os ficheiros de configuração do VS Code (`.vscode/tasks.json`) já incluem tarefas para facilitar este processo.

1.  **Crie o diretório de build**:

    ```bash
    mkdir build
    cd build
    ```

2.  **Execute o CMake e o Ninja**:

    ```bash
    cmake ..
    ninja
    ```

3.  **Carregue o firmware**:

      * Coloque o seu Pico W em modo bootloader (pressionando o botão BOOTSEL ao ligá-lo ao computador).
      * Copie o ficheiro `Estacao_Metereologica_IoT.uf2` gerado no diretório `build` para o dispositivo de armazenamento massivo do Pico.

## 🚀 Como Utilizar

### Display OLED

  * Ao iniciar, o display mostrará os valores de temperatura, pressão, humidade e altitude.
  * **Botão A**: Pressione para alternar entre a página de dados principais e a página que exibe os limites de alarme configurados.
  * **Botão B**: Pressione para ativar ou desativar o alarme sonoro (buzzer).

### Interface Web

1.  Após a inicialização, o endereço IP do Pico W será exibido no terminal serial.
2.  Abra um navegador web e aceda ao endereço IP fornecido.
3.  Na página, pode:
      * Ver os dados dos sensores em tempo real.
      * Acompanhar os gráficos com o histórico das últimas medições.
      * Aceder à secção "Configurações de Limites" para definir os valores de alarme para temperatura e humidade. Clique em "Salvar Limites" para aplicar as novas configurações.

### Sistema de Alarme

  * O **LED Verde** ficará aceso enquanto as medições estiverem dentro dos limites definidos.
  * Se qualquer medição de temperatura ou humidade ultrapassar os limites, o **LED Vermelho** acenderá e, se estiver ativo, o **buzzer** soará um alarme.

## 📂 Estrutura do Projeto

```
.
├── Estacao_Metereologica_IoT.c  # Ficheiro principal da aplicação
├── CMakeLists.txt               # Ficheiro de build do CMake
├── lib/                         # Bibliotecas para os periféricos
│   ├── aht20.c/h                  # Driver do sensor AHT20
│   ├── bmp280.c/h                 # Driver do sensor BMP280
│   ├── buzzer.c/h                 # Controlo do buzzer
│   ├── font.h                     # Fonts para display
│   ├── ssd1306.c/h                # Driver do display OLED
│   └── webserver/
│         ├── lwipopts.h
└──       └── website.c/h            # Lógica do servidor e conteúdo da página web
```
