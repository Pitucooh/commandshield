// processador_comandos.c
#include "processador_comandos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ferramentas_redes.h"
#include "ferramentas_cripto.h"
#include "ajuda_usuario.h"

void display_welcome_banner() {
        printf("\n");
        printf("  ██████╗  ██████╗ ███╗   ███╗███╗   ███╗ █████╗ ███╗   ██╗██████╗       _________________ \n");
        printf(" ██╔════╝ ██╔═══██╗████╗ ████║████╗ ████║██╔══██╗████╗  ██║██╔══██╗     |       | |       | \n");
        printf(" ██║      ██║   ██║██╔████╔██║██╔████╔██║███████║██╔██╗ ██║██║  ██║     |       | |       | \n");
        printf(" ██║      ██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██║╚██╗██║██║  ██║     |       | |       | \n");
        printf(" ╚██████╗ ╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║ ╚████║██████╔╝     |_______| |_______| \n");
        printf("  ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝      |_______   _______| \n");
        printf("                                                                        |       | |       | \n");
        printf("  ███████╗██╗  ██╗██╗███████╗██╗     ██████╗                            |       | |       | \n");
        printf("  ██╔════╝██║  ██║██║██╔════╝██║     ██╔══██╗                            \\      | |      / \n");
        printf("  ███████╗███████║██║█████╗  ██║     ██║  ██║                             \\     | |     / \n");
        printf("  ╚════██║██╔══██║██║██╔══╝  ██║     ██║  ██║                              \\    | |    / \n");
        printf("  ███████║██║  ██║██║███████╗███████╗██████╔╝                               \\   | |   / \n");
        printf("  ╚══════╝╚═╝  ╚═╝╚═╝╚══════╝╚══════╝╚═════╝                                 \\  | |  / \n");
        printf("                                                                              \\ | | / \n");
        printf("                                                                               \\| |/ \n");
        printf("                                                                                \\_/ \n");
        printf("\n");
        printf("=======================================================================================\n");
        printf("                                 Faça seu Login!                                     \n\n");
		printf("=======================================================================================\n");
	}

void init_system() {
    // Inicializa recursos do sistema
    printf("Inicializando CommandShield...\n");
    // Carregar configurações, inicializar módulos, etc.

    // Exibir o banner de boas-vindas
    display_welcome_banner();
}

void cleanup_system() {
    // Libera recursos
    printf("Finalizando CommandShield...\n");
}

int process_command(const char* command) {
    // Verifica se é comando de saída
    if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
        return 0;
    }

    // Comando de ajuda com argumentos
    if (strncmp(command, "help ", 5) == 0) {
        // Extrair o comando específico após "help "
        const char* specific_cmd = command + 5;  // Pula os 5 caracteres de "help "
        show_command_help(specific_cmd);
        return 1;
    }

    // Comando de ajuda sem argumentos
    if (strcmp(command, "help") == 0) {
        show_help_menu();
        return 1;
    }

    // Comandos de rede
    if (strncmp(command, "scan", 4) == 0) {
        return handle_network_scan(command);
    }

    // Comandos de criptografia
    if (strncmp(command, "encrypt", 7) == 0 || strncmp(command, "decrypt", 7) == 0) {
        return handle_crypto_command(command);
    }

    // Comando não reconhecido
    printf("Comando não reconhecido. Digite 'help' para ver a lista de comandos disponíveis.\n");
    return 1;
}