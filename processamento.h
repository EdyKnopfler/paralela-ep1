#ifndef DEF_PROCESSAMENTO

#define DEF_PROCESSAMENTO

#include <stdio.h>
#include <regex.h>

void inicializacao();
void processar(char *arq, FILE *f, regex_t regex);

#endif
