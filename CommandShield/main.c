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

    // Exibir o banner de boas-vindas após autenticação bem-sucedida
    display_welcome_banner();

    // Loop principal do programa
    while (status) {
        printf("CommandShield> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Tratar erro ou EOF
            printf("Erro ao ler comando. Encerrando.\n");
            break;
        }

        // Remover quebra de linha
        command[strcspn(command, "\n")] = 0;

        // Ignorar comandos vazios
        if (command[0] == '\0') {
            continue;
        }

        // Processar comando
        status = process_command(command);

        // Registrar log (exceto para comando "history" para evitar poluição)
        if (strcmp(command, "history") != 0) {
            log_command(command);
        }
    }

    // Finalização do sistema
    cleanup_system();

    // Pausa opcional antes de encerrar (útil se estiver executando fora do VS)
    printf("\nPressione Enter para encerrar...");
    getchar();

    return 0;
}