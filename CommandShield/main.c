// main.c - Ponto de entrada principal do programa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processador_comandos.h"
#include "gerenciamento_usuarios.h"
#include "logging.h"

int main() {
    char command[256];
    int status = 1;

    // Inicialização do sistema
    init_system();

    // Autenticação de usuário
    if (!authenticate_user()) {
        printf("Falha na autenticação. Encerrando o programa.\n");
        return 1;
    }

    // Loop principal do programa
    while (status) {
        printf("CommandShield> ");
        fgets(command, sizeof(command), stdin);

        // Remover quebra de linha
        command[strcspn(command, "\n")] = 0;

        // Processar comando
        status = process_command(command);

        // Registrar log
        log_command(command);
    }

    // Finalização do sistema
    cleanup_system();

    return 0;
}