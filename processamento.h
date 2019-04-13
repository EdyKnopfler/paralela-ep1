#ifndef DEF_PROCESSAMENTO

#define DEF_PROCESSAMENTO

#include <stdio.h>
#include <regex.h>

#include "bufsaida.h"

void inicializacao();
buffer_saida *processar(char *arq, FILE *f, regex_t regex);

#endif
