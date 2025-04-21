// gerenciamento_usuarios.c
#include "gerenciamento_usuarios.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Usuário atual
static User current_user;

int authenticate_user() {
    char username[50];
    char password[50];

    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    // Verificar no banco de dados (simplificado p/ exp)
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        // Corrigido: especificar o tamanho do destino
        strcpy_s(current_user.username, sizeof(current_user.username), username);
        current_user.is_admin = 1;
        return 1;
    }
    else if (strcmp(username, "user") == 0 && strcmp(password, "user123") == 0) {
        // Corrigido: especificar o tamanho do destino
        strcpy_s(current_user.username, sizeof(current_user.username), username);
        current_user.is_admin = 0;
        return 1;
    }

    return 0;
}

int is_admin_user() {
    return current_user.is_admin;
}