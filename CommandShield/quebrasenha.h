#ifndef QUEBRASENHA_H
#define QUEBRASENHA_H
#include <stdio.h>
#define TAM_MAX 30 // tamanho máximo
#define CONT_CHAR 92 // contador de chatacteres

void quebra_senha(const char* senha, int max_tentativas);
void iniciar_quebra_senha();

#endif
