#include "processador_comandos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ferramentas_redes.h"
#include "ferramentas_cripto.h"
#include "ajuda_usuario.h"
#include <time.h>
#include "teste.h"

void run_terminal_tests() {
    printf("\n=== TESTES AUTOMÁTICOS ===\n");

    // [1] Teste de Criptografia
    printf("\n[1] Testando criptografia...\n");

    char original[] = "Senha123";
    char special[] = "Abc!@#123";
    char encrypted[256] = { 0 };
    char decrypted[256] = { 0 };
    int key = 5;

    encrypt_string(original, encrypted, key);
    decrypt_string(encrypted, decrypted, key);

    printf("Original: %s\nCriptografado: %s\nDescriptografado: %s\n", original, encrypted, decrypted);
    if (strcmp(original, decrypted) == 0) {
        printf("✓ Criptografia básica OK\n");
    }
    else {
        printf("✗ Falha na criptografia básica\n");
    }

    encrypt_string(special, encrypted, key);
    decrypt_string(encrypted, decrypted, key);
    printf("Texto com símbolos: %s\nCriptografado: %s\nDescriptografado: %s\n", special, encrypted, decrypted);
    if (strcmp(special, decrypted) == 0) {
        printf("✓ Criptografia com símbolos OK\n");
    }
    else {
        printf("✗ Falha na criptografia com símbolos\n");
    }

    encrypt_string(original, encrypted, -3);
    decrypt_string(encrypted, decrypted, -3);
    printf("Chave negativa - Criptografado: %s | Descriptografado: %s\n", encrypted, decrypted);
    if (strcmp(original, decrypted) == 0) {
        printf("✓ Criptografia com chave negativa OK\n");
    }
    else {
        printf("✗ Falha com chave negativa\n");
    }

    // [2] Teste de rede (ping)
    printf("\n[2] Testando rede (ping localhost)...\n");
    if (ping_host("127.0.0.1")) {
        printf("✓ Ping localhost bem-sucedido\n");
    }
    else {
        printf("✗ Falha no ping localhost\n");
    }

    if (ping_host("256.256.256.256") == 0) {
        printf("✓ Ping com IP inválido corretamente tratado\n");
    }
    else {
        printf("✗ Ping com IP inválido deu sucesso indevidamente\n");
    }

    // [3] Teste de histórico
    printf("\n[3] Testando histórico...\n");
    log_command("comando_teste_1");
    log_command("comando_teste_2");
    printf("Exibindo histórico:\n");
    view_command_history();

    // [4] Teste de escaneamento de portas
    printf("\n[4] Testando escaneamento de portas...\n");
    clock_t inicio = clock();
    handle_network_scan("scan 127.0.0.1 79 81");
    clock_t fim = clock();
    double duracao = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("✓ Scan finalizado em %.2f segundos\n", duracao);

    // [5] Teste de sistema de ajuda
    printf("\n[5] Testando sistema de ajuda...\n");
    printf("Ajuda geral:\n");
    process_help_command("help");

    printf("Ajuda para comando válido:\n");
    process_help_command("help ping");

    printf("Ajuda para comando inválido:\n");
    process_help_command("help banana");

    printf("\n=== FIM DOS TESTES ===\n");
}
