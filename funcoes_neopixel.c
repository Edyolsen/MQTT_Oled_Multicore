#include "funcoes_neopixel.h"
#include <stdlib.h>
#include <time.h>

// Inicializa o gerador de números aleatórios
void inicializar_aleatorio(void) {
    srand((unsigned int)time(NULL));
}

// Gera um número aleatório no intervalo [min, max]
uint16_t numero_aleatorio(uint16_t min, uint16_t max) {
    if (max <= min) return min;
    return (rand() % (max - min + 1)) + min;
}
