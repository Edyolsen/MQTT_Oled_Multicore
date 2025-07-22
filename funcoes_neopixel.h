#ifndef FUNCOES_NEOPIXEL_H
#define FUNCOES_NEOPIXEL_H

#include <stdint.h>

// Inicializa o gerador de números aleatórios
void inicializar_aleatorio(void);

// Gera um número aleatório no intervalo [min, max]
uint16_t numero_aleatorio(uint16_t min, uint16_t max);

#endif
