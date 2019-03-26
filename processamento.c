#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "bufsaida.h"

#define FALSE 0
#define TRUE 1  

pthread_mutex_t mutex_tela;

struct controle_thread {
    int possuo_tela;            /* Estou de posse da tela? */
    buffer_saida *buf_saida;    /* Onde guardo a saída quando não tenho a tela */
    char *arquivo;              /* Arquivo que esta thread está processando no momento */
};

typedef struct controle_thread controle_thread;

void realocar(char **buf_texto, int *tam_buf);
void produzirSaida(controle_thread *controle, int num_linhas);
void descarregarBuffer(buffer_saida *buf_saida);

void inicializacao() {
    pthread_mutex_init(&mutex_tela, NULL);
}

void processar(char *arq, FILE *f, regex_t regex) {
    int pos = 0, tam_buf = 128, cont_linhas = 0, c;
    char *buf_texto = malloc(tam_buf);
    controle_thread controle;  /* Alocado na pilha - 1 por thread */
    
    controle.possuo_tela = FALSE;
    controle.buf_saida = criarBufferSaida();
    controle.arquivo = arq;
    
    while ((c = getc(f)) != EOF) {
        if (pos == tam_buf)
            realocar(&buf_texto, &tam_buf);
        if (c != '\n')
            buf_texto[pos++] = c;
        else {
            buf_texto[pos] = '\0';
            if ((regexec(&regex, buf_texto, 0, (regmatch_t *)NULL, 0)) == 0) {
                /* Aqui tenta obter o lock da tela */
                produzirSaida(&controle, cont_linhas);
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
            produzirSaida(&controle, cont_linhas);
    }
    
    /* Quando vou poder imprimir? */
    if (!bufferEstaVazio(controle.buf_saida)) {
        if (controle.possuo_tela == FALSE) pthread_mutex_lock(&mutex_tela);
        controle.possuo_tela = TRUE;
        descarregarBuffer(controle.buf_saida);
    }
    
    if (controle.possuo_tela == TRUE) pthread_mutex_unlock(&mutex_tela);
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

void produzirSaida(controle_thread *controle, int num_linhas) {
    if (controle->possuo_tela == TRUE) {
        printf("%s: %d\n", controle->arquivo, num_linhas);
    }
    else {
        if (pthread_mutex_trylock(&mutex_tela) == 0) {
            controle->possuo_tela = TRUE;
            descarregarBuffer(controle->buf_saida);
            printf("%s: %d\n", controle->arquivo, num_linhas);
        }
        else
            novoItemSaida(controle->buf_saida, controle->arquivo, num_linhas);
    }
}

void descarregarBuffer(buffer_saida *buf) {
    item_saida *item;
    FOREACH(buf, item, printf("%s: %d\n", item->arquivo, item->num_linhas));
    /* Muuuuuuuuuito preocupado com a alocação, vai jogar tudo fora :P */
    buf->cabeca->prox = NULL;
    buf->fim = buf->cabeca;
}
