#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "bufsaida.h"

#define FALSE 0
#define TRUE 1

void realocar(char **buf_texto, int *tam_buf);

buffer_saida *processar(char *arq, FILE *f, regex_t regex) {
    int pos = 0, tam_buf = 128, cont_linhas = 0, c;
    char *buf_texto = malloc(tam_buf);
    buffer_saida *buff = criarBufferSaida();
    
    while ((c = getc(f)) != EOF) {
        if (pos == tam_buf)
            realocar(&buf_texto, &tam_buf);
        if (c != '\n')
            buf_texto[pos++] = c;
        else {
            buf_texto[pos] = '\0';
            if ((regexec(&regex, buf_texto, 0, (regmatch_t *)NULL, 0)) == 0) {
                novoItemSaida(buff, arq, cont_linhas);
            }
            cont_linhas++;
            pos = 0;
        }
    }
    
    /* Final do arquivo */
    if (pos > 0) {
        if (pos == tam_buf) realocar(&buf_texto, &tam_buf);
        buf_texto[pos] = '\0';
        if ((regexec(&regex, buf_texto, 0, (regmatch_t *)NULL, 0)) == 0)
            novoItemSaida(buff, arq, cont_linhas);
    }
    
    return buff;
}

void realocar(char **buf_texto, int *tam_buf) {
    char *buf_velho;
    int i;
    buf_velho = *buf_texto;
    *buf_texto = malloc(*tam_buf * 2);
    for (i = 0; i < *tam_buf; i++) (*buf_texto)[i] = buf_velho[i];
    (*buf_texto)[i] = '\0';
    free(buf_velho);
    *tam_buf *= 2;
}
