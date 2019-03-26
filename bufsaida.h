#ifndef DEF_BUFSAIDA

#define DEF_BUFSAIDA

struct item_saida {
    char *arquivo;
    int num_linhas;
    struct item_saida *prox;
};

typedef struct item_saida item_saida;

struct buffer_saida {
    item_saida *cabeca;
    item_saida *fim;
};

typedef struct buffer_saida buffer_saida;

buffer_saida *criarBufferSaida();
void novoItemSaida(buffer_saida *buf, char *arquivo, int num_linhas);
int bufferEstaVazio(buffer_saida *buf);

#define FOREACH(buf, item, codigo) for (item = buf->cabeca->prox; item != NULL; item = item->prox) codigo

#endif
