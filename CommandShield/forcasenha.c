#include <stdio.h>
#include "forcasenha.h"
#include <ctype.h>
#include <string.h>

void forca_da_senha(char senha[]) {
    // variáveis para analisar o nível de força da senha
    int tamanhoSenha = strlen(senha);
    int letraMinuscula = 0;
    int letraMaiuscula = 0;
    int numero = 0;
    int caracterEspecial = 0;

    for (int i = 0; senha[i]; i++) {
        if (islower(senha[i])) // contador para letra minúscula
            letraMinuscula++;
        if (isupper(senha[i])) // contador para letra maiúscula
            letraMaiuscula++;
        if (isdigit(senha[i])) // contador para número
            numero++;
        if (ispunct(senha[i])) // contador para caracter especial
            caracterEspecial++;
    }
    // testes para a força da senha e com dicas
    if (tamanhoSenha < 8) {
        printf("Senha MUITO FRACA (Recomendado adicionar MAIS letras, letras maiúsculas, números e caracteres especiais)\n");
    }
    else if (tamanhoSenha <= 8 && letraMaiuscula == 0 && caracterEspecial == 0) {
        printf("Senha FRACA (Recomendado adicionar letras maiúsculas, números e caracteres especiais)\n");
    }
    else if (tamanhoSenha >= 8 && tamanhoSenha < 12 && letraMinuscula >= 1 && letraMaiuscula >= 1 && numero >= 1 && caracterEspecial >= 1) {
        printf("Senha BOA (Recomendado adicionar números e caracteres especiais)\n");
    }
    else {
        printf("Senha MUITO FORTE\n");
    }
}

void iniciar_forca_da_senha() {
    char senha[SENHA_MAX];

    printf("Digite a senha a ser testada o nível de força (até %d caracteres): ", SENHA_MAX);
    scanf_s("%s", senha, (unsigned)sizeof(senha));

    forca_da_senha(senha);
}