// ajuda_usuario.c
#include "ajuda_usuario.h"
#include <stdio.h>
#include <string.h>

void show_help_menu() {
    printf("\n=== CommandShield Help Menu ===\n");
    printf("Comandos disponíveis:\n\n");
    printf("NAVEGAÇÃO E SISTEMA:\n");
    printf("  help                 - Mostra este menu de ajuda\n");
    printf("  help [comando]       - Mostra ajuda detalhada sobre um comando específico\n");
    printf("  exit, quit           - Sai do programa\n\n");
    printf("FERRAMENTAS DE REDE:\n");
    printf("  scan [host] [p1] [p2] - Escaneia portas de p1 até p2 no host especificado\n");
    printf("  ping [host]          - Verifica se um host está acessível\n\n");
    printf("CRIPTOGRAFIA:\n");
    printf("  encrypt [texto] [chave] - Criptografa um texto usando chave numérica\n");
    printf("  decrypt [texto] [chave] - Descriptografa um texto usando chave numérica\n\n");
    printf("HISTÓRICO:\n");
    printf("  history              - Mostra histórico de comandos executados\n\n");
    printf("Digite 'help [comando]' para obter informações detalhadas sobre um comando específico.\n");
}

void show_command_help(const char* command) {
    if (strcmp(command, "scan") == 0) {
        printf("\n=== Ajuda: scan ===\n");
        printf("Uso: scan [host] [porta_inicial] [porta_final]\n\n");
        printf("Descrição: Escaneia um intervalo de portas em um host específico para verificar quais estão abertas.\n");
        printf("Exemplo: scan 192.168.1.1 1 1000\n");
        printf("  Este comando escaneará as portas de 1 a 1000 no host 192.168.1.1\n");
    }
    else if (strcmp(command, "ping") == 0) {
        printf("\n=== Ajuda: ping ===\n");
        printf("Uso: ping [host]\n\n");
        printf("Descrição: Verifica se um host está acessível na rede.\n");
        printf("Exemplo: ping 192.168.1.1\n");
    }
    else if (strcmp(command, "encrypt") == 0) {
        printf("\n=== Ajuda: encrypt ===\n");
        printf("Uso: encrypt [texto] [chave]\n\n");
        printf("Descrição: Criptografa um texto usando uma cifra simples com a chave fornecida.\n");
        printf("Exemplo: encrypt HelloWorld 3\n");
    }
    else if (strcmp(command, "decrypt") == 0) {
        printf("\n=== Ajuda: decrypt ===\n");
        printf("Uso: decrypt [texto] [chave]\n\n");
        printf("Descrição: Descriptografa um texto que foi criptografado usando a mesma chave.\n");
        printf("Exemplo: decrypt KhoorZruog 3\n");
    }
    else if (strcmp(command, "history") == 0) {
        printf("\n=== Ajuda: history ===\n");
        printf("Uso: history\n\n");
        printf("Descrição: Mostra o histórico de comandos executados na sessão atual.\n");
    }
    else {
        printf("Ajuda não disponível para o comando '%s'. Digite 'help' para ver a lista de comandos.\n", command);
    }
}

// Nova função para processar comandos de ajuda
int process_help_command(const char* command) {
    // Se for exatamente "help" sem argumentos
    if (strcmp(command, "help") == 0) {
        show_help_menu();
        return 1;
    }

    // Se tem argumentos adicionais (help [comando])
    const char* space = strchr(command, ' ');
    if (space != NULL && *(space + 1) != '\0') {
        // Extrair o comando específico que vem após "help "
        const char* specific_cmd = space + 1;
        show_command_help(specific_cmd);
        return 1;
    }

    // Se chegou aqui, o formato está incorreto
    printf("Formato inválido para o comando help. Use 'help' ou 'help [comando]'.\n");
    return 1;
}