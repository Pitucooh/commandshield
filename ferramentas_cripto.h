// ferramentas_cripto.h
#ifndef FERRAMENTAS_CRIPTO_H
#define FERRAMENTAS_CRIPTO_H

// Processa comandos de criptografia
int handle_crypto_command(const char* command);

// Criptografa uma string usando cifra simples (exemplo: Caesar)
void encrypt_string(const char* input, char* output, int key);

// Descriptografa uma string
void decrypt_string(const char* input, char* output, int key);

#endif