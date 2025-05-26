// processador_comandos.c
#include "processador_comandos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ferramentas_redes.h"
#include "ferramentas_cripto.h"
#include "ajuda_usuario.h"
#include "logging.h"
#include "Info_Hardware.h"
#include "teste.h"
#include "forcasenha.h"
#include "quebrasenha.h"


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
    printf("           Faça seu Login!  (Para ver os comandos disponíveis, digite 'help')         \n\n");
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
    // Libera recursos do sistema
    printf("\nEncerrando CommandShield...\n");
    // Fechar conexões, liberar memória, etc.
}

int process_command(const char* command) {
    // Loga o comando antes de processar (exceto history e comandos vazios)
    if (command && command[0] != '\0') {
        log_command(command);
    }
    else {
        return 1; // Ignora comando vazio
    }

    // Verifica se é comando de saída
    if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
        return 0; // Sinaliza para sair do loop principal
    }

    // Comando de ajuda (com ou sem argumentos)
    if (strncmp(command, "help", 4) == 0) {
        return process_help_command(command); // Deixa a função de ajuda tratar os argumentos
    }

    // Comandos de rede
    if (strncmp(command, "scan", 4) == 0 || strncmp(command, "ping", 4) == 0) {
        return handle_network_scan(command);
    }

    // Comandos de criptografia
    if (strncmp(command, "encrypt", 7) == 0 || strncmp(command, "decrypt", 7) == 0) {
        return handle_crypto_command(command);
    }

    // Comando histórico
    if (strcmp(command, "history") == 0) {
        view_command_history();
        return 1;
    }

    // Comandos de Hardware
    if (strcmp(command, "drivers") == 0) {
        return handle_hardware_command("drivers");
    }
    if (strcmp(command, "meminfo") == 0) {
        return handle_hardware_command("meminfo");
    }
    if (strcmp(command, "cpuinfo") == 0) {
        return handle_hardware_command("cpuinfo");
    }
    if (strcmp(command, "diskinfo") == 0) {
        return handle_hardware_command("diskinfo");
    }
    if (strcmp(command, "netinfo") == 0) {
        return handle_hardware_command("netinfo");
    }
    if (strcmp(command, "usbinfo") == 0) {
        return handle_hardware_command("usbinfo");
    }
    if (strcmp(command, "procinfo") == 0) {
        return handle_hardware_command("procinfo");
    }

    // Teste branco (supondo que exista)
    if (strcmp(command, "test") == 0) {
        // run_terminal_tests(); // Chamada comentada se não implementada
        printf("Comando 'test' reconhecido (implementação pendente).\n");
        return 1;
    }

    // Teste caixa preta (supondo que exista)
    if (strcmp(command, "test-black") == 0) {
        // run_black_box_tests(); // Chamada comentada se não implementada
        printf("Comando 'test-black' reconhecido (implementação pendente).\n");
        return 1;
    }
    // teste para quebrar senha
    if (strcmp(command, "quebra-senha") == 0) {
        iniciar_quebra_senha();
        return 1;
    }
    // teste para força da senha
    if (strcmp(command, "forca-senha") == 0) {
        iniciar_forca_da_senha();
        return 1;
    }

    // Comando não reconhecido
    printf("Comando não reconhecido: '%s'. Digite 'help' para ver a lista de comandos disponíveis.\n", command);
    return 1; // Continua o loop
}
