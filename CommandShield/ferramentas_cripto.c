// ferramentas_cripto.c
#include "ferramentas_cripto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int handle_crypto_command(const char* command) {
    char subcommand[20] = { 0 };
    char text[256] = { 0 };
    int key = 0;
    char result[256] = { 0 };

    // Parse comando com verificação segura
    if (sscanf_s(command, "%19s %255s %d",
        subcommand, (unsigned int)sizeof(subcommand),
        text, (unsigned int)sizeof(text),
        &key) != 3) {
        printf("Uso incorreto. Exemplos:\n");
        printf("  encrypt HelloWorld 3 - Criptografa texto com chave 3\n");
        printf("  decrypt KhoorZruog 3 - Descriptografa texto com chave 3\n");
        return 1;
    }

    printf("Comando: %s, Texto: %s, Chave: %d\n", subcommand, text, key);

    if (strcmp(subcommand, "encrypt") == 0) {
        encrypt_string(text, result, key);
        printf("Texto original: %s\n", text);
        printf("Texto criptografado: %s\n", result);
    }
    else if (strcmp(subcommand, "decrypt") == 0) {
        decrypt_string(text, result, key);
        printf("Texto original: %s\n", text);
        printf("Texto descriptografado: %s\n", result);
    }
    else {
        printf("Subcomando desconhecido. Use 'encrypt' ou 'decrypt'.\n");
    }

    return 1;
}

// Cifra de César (exemplo simples)
void encrypt_string(const char* input, char* output, int key) {
    if (!input || !output) {
        printf("Erro: ponteiros nulos na função encrypt_string\n");
        return;
    }

    // Garantir que a chave esteja no intervalo correto (0-25)
    key = key % 26;
    if (key < 0) key += 26;

    printf("Aplicando cifra com chave: %d\n", key);

    size_t i = 0;
    while (input[i] != '\0' && i < 255) { // Evitar overflow
        char c = input[i];

        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            output[i] = (char)(((c - base + key) % 26) + base);
        }
        else {
            output[i] = c; // Manter caracteres não-alfabéticos inalterados
        }

        i++;
    }

    output[i] = '\0'; // Garantir terminação da string
    printf("Criptografia concluída: %s\n", output);
}

void decrypt_string(const char* input, char* output, int key) {
    if (!input || !output) {
        printf("Erro: ponteiros nulos na função decrypt_string\n");
        return;
    }

    // Para descriptografar, usamos o mesmo algoritmo com chave negativa
    // Garantimos que a chave esteja no intervalo correto (0-25)
    int decryptKey = 26 - (key % 26);
    if (decryptKey == 26) decryptKey = 0;

    printf("Aplicando decifra com chave inversa: %d\n", decryptKey);
    encrypt_string(input, output, decryptKey);
}