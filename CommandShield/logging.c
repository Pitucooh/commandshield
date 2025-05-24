#include "logging.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "gerenciamento_usuarios.h"

// Usuário atual (deve ser declarado em gerenciamento_usuarios.c e extern aqui)
extern User current_user;

void log_command(const char* command) {
    if (command == NULL || command[0] == '\0') {
        return;
    }

    if (strcmp(command, "history") == 0) {
        return;
    }

    FILE* log_file = NULL;
    errno_t err = fopen_s(&log_file, "command_log.txt", "a");

    if (log_file != NULL && err == 0) {
        time_t now = time(NULL);
        char time_str[100] = { 0 };

        if (ctime_s(time_str, sizeof(time_str), &now) == 0) {
            time_str[strcspn(time_str, "\n")] = 0;

            // Novo formato de log: [DATA] USER: username | COMMAND: comando
            fprintf(log_file, "[%s] USER: %s | COMMAND: %s\n",
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
        char line[512];
        int count = 0;

        printf("\n=== Histórico de Comandos de %s ===\n", current_user.username);

        while (fgets(line, sizeof(line), log_file)) {
            // Verifica se a linha é do usuário atual
            char search_pattern[100];
            snprintf(search_pattern, sizeof(search_pattern), "USER: %s |", current_user.username);

            if (strstr(line, search_pattern) != NULL) {
                count++;
                printf("%3d  %s", count, line);
            }
        }

        if (count == 0) {
            printf("Nenhum comando no histórico para este usuário.\n");
        }

        printf("\nTotal de comandos: %d\n", count);

        fclose(log_file);
    }
    else {
        printf("Nenhum histórico de comandos encontrado.\n");
    }
}
