cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(Estacao_Metereologica_IoT C CXX ASM)
pico_sdk_init()

add_executable(${PROJECT_NAME} Estacao_Metereologica_IoT.c 
            lib/aht20.c
            lib/bmp280.c
            lib/buzzer.c
            lib/ssd1306.c
            lib/webserver/website.c
            )
pico_set_program_name(${PROJECT_NAME} "Estacao_Metereologica_IoT")
pico_set_program_version(${PROJECT_NAME} "0.1")
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        pico_multicore
        hardware_i2c
        hardware_pwm
        pico_cyw43_arch_lwip_threadsafe_background
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib
        ${CMAKE_CURRENT_LIST_DIR}/lib/webserver
)
pico_add_extra_outputs(${PROJECT_NAME})

