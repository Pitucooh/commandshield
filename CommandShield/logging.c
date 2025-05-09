// logging.c
#include "logging.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "gerenciamento_usuarios.h"

// Usuário atual (deve ser declarado em gerenciamento_usuarios.c e extern aqui)
extern User current_user;

void log_command(const char* command) {
    // Não registra comandos vazios
    if (command == NULL || command[0] == '\0') {
        return;
    }

    // Não registra o próprio comando history para evitar acúmulo
    if (strcmp(command, "history") == 0) {
        return;
    }

    FILE* log_file = NULL;
    errno_t err = fopen_s(&log_file, "command_log.txt", "a");

    if (log_file != NULL && err == 0) {
        time_t now = time(NULL);
        char time_str[100] = { 0 };

        // Usar ctime_s em vez de ctime para segurança
        if (ctime_s(time_str, sizeof(time_str), &now) == 0) {
            // Remover a quebra de linha do final do time_str
            time_str[strcspn(time_str, "\n")] = 0;

            // Registrar o comando com timestamp, usuário e comando
            fprintf(log_file, "[%s] %s: %s\n",
                time_str,
                current_user.username,
                command);
        }

        fclose(log_file);
    }
    else {
        printf("Aviso: Não foi possível registrar o comando no histórico.\n");
    }
}

void view_command_history() {
    FILE* log_file = NULL;
    errno_t err = fopen_s(&log_file, "command_log.txt", "r");

    if (log_file != NULL && err == 0) {
        char line[256];
        int count = 0;

        printf("\n=== Histórico de Comandos ===\n");

        // Ler e exibir cada linha do arquivo de log
        while (fgets(line, sizeof(line), log_file)) {
            count++;
            printf("%3d  %s", count, line);
        }

        if (count == 0) {
            printf("Nenhum comando no histórico.\n");
        }

        printf("\nTotal de comandos: %d\n", count);

        fclose(log_file);
    }
    else {
        printf("Nenhum histórico de comandos encontrado.\n");
    }
}