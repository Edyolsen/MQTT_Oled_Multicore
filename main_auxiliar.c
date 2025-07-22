
#include "fila_circular.h"
#include "rgb_pwm_control.h"
#include "configura_geral.h"
#include "oled_utils.h"
#include "ssd1306_i2c.h"
#include "mqtt_lwip.h"
#include "lwip/ip_addr.h"
#include "pico/multicore.h"
#include <stdio.h>
#include "estado_mqtt.h"
#include "funcoes_neopixel.h"

void espera_usb() {
    while (!stdio_usb_connected()) {
        sleep_ms(200);
    }
    printf("Conexão USB estabelecida!\n");
}

void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";

    if (msg.tentativa == 0x9999) {
        if (msg.status == 0) {
            ssd1306_draw_utf8_multiline(buffer_oled, 0, 32, "ACK do PING OK");

            static bool aleatorio_inicializado = false;
            if (!aleatorio_inicializado) {
                inicializar_aleatorio();
                aleatorio_inicializado = true;
            }

            uint16_t r, g, b;
            do {
                r = numero_aleatorio(0, PWM_STEP);
                g = numero_aleatorio(0, PWM_STEP);
                b = numero_aleatorio(0, PWM_STEP);
            } while (r == 0 && g == PWM_STEP && b == 0);

            set_rgb_pwm(r, g, b);
            sleep_ms(1000);

        } else {
            ssd1306_draw_utf8_multiline(buffer_oled, 0, 32, "ACK do PING FALHOU");
            set_rgb_pwm(65535, 0, 0);
        }
        render_on_display(buffer_oled, &area);
        return;
    }

    switch (msg.status) {
        case 0:
            descricao = "INICIALIZANDO";
            set_rgb_pwm(PWM_STEP, 0, 0);
            break;
        case 1:
            descricao = "CONECTADO";
            set_rgb_pwm(0, PWM_STEP, 0);
            break;
        case 2:
            descricao = "FALHA";
            set_rgb_pwm(0, 0, PWM_STEP);
            break;
        default:
            descricao = "DESCONHECIDO";
            set_rgb_pwm(PWM_STEP, PWM_STEP, PWM_STEP);
            break;
    }

    char linha_status[32];
    snprintf(linha_status, sizeof(linha_status), "Status do Wi-Fi : %s", descricao);

    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, linha_status);
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Status: %s (%s)\n", descricao, msg.tentativa > 0 ? descricao : "evento");
}

void tratar_ip_binario(uint32_t ip_bin) {
    char ip_str[20];
    uint8_t ip[4];

    ip[0] = (ip_bin >> 24) & 0xFF;
    ip[1] = (ip_bin >> 16) & 0xFF;
    ip[2] = (ip_bin >> 8) & 0xFF;
    ip[3] = ip_bin & 0xFF;

    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    oled_clear(buffer_oled, &area);
    ssd1306_draw_utf8_string(buffer_oled, 0, 0, ip_str);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Endereço IP: %s\n", ip_str);
    ultimo_ip_bin = ip_bin;
}

void exibir_status_mqtt(const char *texto) {
    ssd1306_draw_utf8_string(buffer_oled, 0, 16, "MQTT: ");
    ssd1306_draw_utf8_string(buffer_oled, 40, 16, texto);
    render_on_display(buffer_oled, &area);

    printf("[MQTT] %s\n", texto);
}

void set_novo_intervalo_ping(uint32_t novo_intervalo) {
    if (novo_intervalo >= 1000 && novo_intervalo <= 60000) {
        intervalo_ping_ms = novo_intervalo;

        char buffer_msg[32];
        snprintf(buffer_msg, sizeof(buffer_msg), "Intervalo: %u ms", novo_intervalo);

        ssd1306_clear_area(buffer_oled, 0, 40, 127, 50);
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 42, buffer_msg);
        render_on_display(buffer_oled, &area);

        printf("[INFO] Intervalo atualizado para %u ms\n", novo_intervalo);
    } else {
        printf("[AVISO] Valor %u fora do intervalo permitido (1000–60000 ms)\n", novo_intervalo);
    }
}
