#include "quebrasenha.h"
#include <string.h>

void quebra_senha(const char* senha, int max_tentativas) {
    const char chars[CONT_CHAR + 1] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1203495687.!-@*_$#/,+%&?;=~^)[\\]`(:<'>|\"";
    int guessc[TAM_MAX] = { 0 };
    char guess[TAM_MAX + 1] = { 0 };
    int tentativas = 0;
    int i, j;

    // testa tamanhos de senha de 1 caracter até o TAM_MAX (tamanho máximo)
    for (int tamanho = 1; tamanho <= TAM_MAX && tentativas < max_tentativas; tamanho++) {
        for (i = 0; i < tamanho; i++) guessc[i] = 0; // início do vetor guessc
        while (tentativas < max_tentativas) {
            // montagem da senha para testar
            for (j = 0; j < tamanho; j++) {
                guess[j] = chars[guessc[j]];
            }
            guess[tamanho] = '\0';
            tentativas++;
            printf("Tentativa %d: %s\n", tentativas, guess);
            if (strcmp(guess, senha) == 0) {
                printf("Senha encontrada: %s (em %d tentativas)\n", guess, tentativas);
                return;
            }
            // tentativas da senha variam da direita para a esquerda e aumenta a combinação atual
            i = tamanho - 1;
            while (i >= 0) {
                guessc[i]++;
                if (guessc[i] >= CONT_CHAR) {
                    guessc[i] = 0;
                    i--;
                }
                else {
                    break;
                }
            }
            if (i < 0) {
                break; // caso todas as combinações forem testadas (difícil de acontecer)
            }
        }
    }
    // caso a senha não seja encontrada dentro do número de tentativas
    printf("Limite de %d tentativas atingido. Senha não encontrada.\n", max_tentativas);
}

void iniciar_quebra_senha() {
    char senha[TAM_MAX];
    int limite;

    printf("Digite a senha a ser quebrada (até %d caracteres): ", TAM_MAX - 1);
    scanf_s("%s", senha, (unsigned)sizeof(senha));
    while (getchar() != '\n'); // limpa o buffer

    printf("Digite o número máximo de tentativas: ");
    scanf_s("%d", &limite);
    while (getchar() != '\n');

    quebra_senha(senha, limite);
}