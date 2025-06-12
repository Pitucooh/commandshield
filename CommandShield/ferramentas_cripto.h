// ferramentas_cripto.h
#ifndef FERRAMENTAS_CRIPTO_H
#define FERRAMENTAS_CRIPTO_H

int handle_crypto_command(const char* command);

void encrypt_string(const char* input, char* output, int key);

void decrypt_string(const char* input, char* output, int key);

#endif