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
    printf("HARDWARE:\n");
    printf("  drivers              - Lista os drivers de dispositivo carregados no sistema\n");
    printf("  meminfo              - Mostra informações detalhadas sobre a memória do sistema\n");
    printf("  cpuinfo              - Mostra informações sobre o processador\n");
    printf("  diskinfo             - Mostra informações sobre os discos do sistema\n");
    printf("  netinfo              - Mostra informações sobre adaptadores e conexões de rede\n");
    printf("  usbinfo              - Lista dispositivos USB conectados\n");
    printf("  procinfo             - Mostra processos em execução no sistema\n\n");
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
    else if (strcmp(command, "drivers") == 0) {
        printf("\n=== Ajuda: drivers ===\n");
        printf("Uso: drivers\n\n");
        printf("Descrição: Lista todos os drivers de dispositivo carregados no sistema operacional.\n");
        printf("Este comando utiliza o utilitário driverquery do Windows para obter informações sobre os drivers ativos.\n");
        printf("Nota: Requer privilégios de administrador para funcionar corretamente.\n");
    }
    else if (strcmp(command, "meminfo") == 0) {
        printf("\n=== Ajuda: meminfo ===\n");
        printf("Uso: meminfo\n\n");
        printf("Descrição: Exibe informações detalhadas sobre a memória do sistema.\n");
        printf("Mostra dados como memória física total, disponível e em uso, memória virtual e informações de paginação.\n");
        printf("Os valores são exibidos em MB e GB para facilitar a leitura.\n");
    }
    else if (strcmp(command, "cpuinfo") == 0) {
        printf("\n=== Ajuda: cpuinfo ===\n");
        printf("Uso: cpuinfo\n\n");
        printf("Descrição: Exibe informações detalhadas sobre o processador do sistema.\n");
        printf("Mostra dados como modelo do processador, número de núcleos físicos e lógicos,\n");
        printf("velocidade máxima e uso atual da CPU.\n");
    }
    else if (strcmp(command, "diskinfo") == 0) {
        printf("\n=== Ajuda: diskinfo ===\n");
        printf("Uso: diskinfo\n\n");
        printf("Descrição: Exibe informações sobre os discos e volumes do sistema.\n");
        printf("Mostra dados como letra da unidade, sistema de arquivos, tamanho total,\n");
        printf("espaço livre e espaço usado (em GB e porcentagem).\n");
    }
    else if (strcmp(command, "netinfo") == 0) {
        printf("\n=== Ajuda: netinfo ===\n");
        printf("Uso: netinfo\n\n");
        printf("Descrição: Exibe informações sobre os adaptadores de rede e conexões.\n");
        printf("Mostra dados como nome do adaptador, endereço MAC, endereços IP,\n");
        printf("gateway e estatísticas de tráfego de rede.\n");
    }
    else if (strcmp(command, "usbinfo") == 0) {
        printf("\n=== Ajuda: usbinfo ===\n");
        printf("Uso: usbinfo\n\n");
        printf("Descrição: Lista todos os dispositivos USB conectados ao sistema.\n");
        printf("Mostra dados como descrição do dispositivo, ID de hardware e fabricante.\n");
        printf("Nota: Requer privilégios de administrador para informações completas.\n");
    }
    else if (strcmp(command, "procinfo") == 0) {
        printf("\n=== Ajuda: procinfo ===\n");
        printf("Uso: procinfo\n\n");
        printf("Descrição: Lista todos os processos em execução no sistema.\n");
        printf("Mostra dados como ID do processo (PID), nome do executável e número de threads.\n");
        printf("Nota: Requer privilégios de administrador para informações completas.\n");
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
