// logging.c
#include "logging.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "gerenciamento_usuarios.h"

extern User current_user; // Declaração externa para acessar o usuário atual

void log_command(const char* command) {
    FILE* log_file = NULL;
    errno_t err = fopen_s(&log_file, "command_log.txt", "a");

    if (log_file != NULL && err == 0) {
        time_t now = time(NULL);
        char time_str[100];

        // Usar ctime_s em vez de ctime
        ctime_s(time_str, sizeof(time_str), &now);
        time_str[strcspn(time_str, "\n")] = 0;

        // Presumindo que current_user é acessível ou que você tem uma função para obter o nome do usuário
        fprintf(log_file, "[%s] User: %s, Command: %s\n",
            time_str, "current_user", command);

        fclose(log_file);
    }
}

void view_command_history() {
    FILE* log_file = NULL;
    errno_t err = fopen_s(&log_file, "command_log.txt", "r");

    if (log_file != NULL && err == 0) {
        char line[256];
        printf("=== Command History ===\n");

        while (fgets(line, sizeof(line), log_file)) {
            printf("%s", line);
        }

        fclose(log_file);
    }
    else {
        printf("No command history found.\n");
    }
}