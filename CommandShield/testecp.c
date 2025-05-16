#include <stdio.h>
#include <string.h>
#include "teste.h"
#include "processador_comandos.h"

void process_user_input(const char* input) {
    process_command(input);
}


void run_black_box_tests() {
    printf("\n=== TESTES DE CAIXA PRETA ===\n");

    // Teste 1: Comando de ajuda
    printf("\n[1] help\n");
    process_user_input("help");

    // Teste 2: Comando válido de ping
    printf("\n[2] ping 127.0.0.1\n");
    process_user_input("ping 127.0.0.1");

    // Teste 3: Comando inválido de ping
    printf("\n[3] ping 256.256.256.256\n");
    process_user_input("ping 256.256.256.256");

    // Teste 4: Comando de escaneamento de porta
    printf("\n[4] scan 127.0.0.1 79 81\n");
    process_user_input("scan 127.0.0.1 79 81");

    // Teste 5: Comando de criptografia (simulado)
    printf("\n[5] encrypt Senha123 5\n");
    process_user_input("encrypt Senha123 5");

    // Teste 6: Comando de decriptografia (simulado)
    printf("\n[6] decrypt Senha123 5\n");
    process_user_input("decrypt Senha123 5");

    // Teste 7: Comando inválido qualquer
    printf("\n[7] comandoInvalido\n");
    process_user_input("comandoInvalido");

    // Teste 8: Histórico
    printf("\n[8] history\n");
    process_user_input("history");

    printf("\n=== FIM DOS TESTES DE CAIXA PRETA ===\n");
}
