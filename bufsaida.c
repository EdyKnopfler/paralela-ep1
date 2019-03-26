#include <stdlib.h>
#include <stdio.h>
#include "bufsaida.h"

buffer_saida *criarBufferSaida() {
    buffer_saida *buf = malloc(sizeof(buffer_saida));
    buf->cabeca = malloc(sizeof(item_saida));
    buf->cabeca->prox = NULL;
    buf->fim = buf->cabeca;
    return buf;
}

void novoItemSaida(buffer_saida *buf, char *arquivo, int num_linhas) {
    buf->fim->prox = malloc(sizeof(item_saida));
    buf->fim = buf->fim->prox;
    buf->fim->arquivo = arquivo;
    buf->fim->num_linhas = num_linhas;
    buf->fim->prox = NULL;
}

int bufferEstaVazio(buffer_saida *buf) {
    return buf->cabeca->prox == NULL;
}
