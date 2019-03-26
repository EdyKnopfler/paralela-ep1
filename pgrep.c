#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include "processamento.h"

struct arquivo {
    char *path;
    struct arquivo *prox;
};

struct arquivo *lista_arquivos, *proximo_arq;
regex_t regex;
pthread_mutex_t mutex_dir;

void listarDiretorio(char *dir);
void *thread();

int main(int argc, char **argv) {
    pthread_t *threads;
    int n_threads, i;
    
    setbuf(stdout, NULL);
    if (argc < 4) {
        fprintf(stderr, "USO: pgrep MAX_THREADS REGEX_PESQUISA CAMINHO_DO_DIRETÓRIO\n");
        return 1;
    }
    
    if (regcomp(&regex, argv[2], REG_EXTENDED|REG_NOSUB) != 0) {
		fprintf(stderr,"erro regcomp\n");
		exit(1);
	}
    
    lista_arquivos = NULL;
    listarDiretorio(argv[3]);
    proximo_arq = lista_arquivos;
    
    pthread_mutex_init(&mutex_dir, NULL);
    n_threads = atoi(argv[1]);
    threads = malloc(n_threads * sizeof(pthread_t));
    inicializacao();

    for (i = 0; i < n_threads; i++)
        pthread_create(&threads[i], NULL, thread, NULL);
    
    for (i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);
    
    return 0;
}

void listarDiretorio(char *dir) {
    DIR *dr;
    struct dirent *de;
    struct arquivo *arquivo;
    size_t tam_atual, tam_novo, tam_total;
    char *novo_path;
    
    dr = opendir(dir);
    while (1) {
        de = readdir(dr);
        if (de == NULL) break;
        tam_atual = strlen(dir);
        tam_novo = strlen(de->d_name);
        tam_total = (tam_atual + tam_novo + 2) * 2;
        novo_path = malloc(tam_total);
        sprintf(novo_path, "%s/%s", dir, de->d_name);
        if (de->d_type == 8) {
            arquivo = malloc(sizeof(struct arquivo));
            arquivo->path = novo_path;
            if (lista_arquivos == NULL) {
                lista_arquivos = arquivo;
                arquivo->prox = NULL;
            }
            else {
                arquivo->prox = lista_arquivos;
                lista_arquivos = arquivo;
            }
        }
        else if (de->d_type == 4 && de->d_name[0] != '.') {
            listarDiretorio(novo_path);
            free(novo_path);
        }
    }
    closedir(dr);
}

void *thread() {
    char *arquivo;
    FILE *f;
    while (1) {
        if (proximo_arq == NULL) return NULL;
        pthread_mutex_lock(&mutex_dir);
        arquivo = proximo_arq->path;
        proximo_arq = proximo_arq->prox;
        pthread_mutex_unlock(&mutex_dir);
        f = fopen(arquivo, "r");
        processar(arquivo, f, regex);
        fclose(f);
    }
}
