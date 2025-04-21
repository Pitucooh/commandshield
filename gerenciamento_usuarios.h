// gerenciamento_usuarios.h
#ifndef GERENCIAMENTO_USUARIOS_H
#define GERENCIAMENTO_USUARIOS_H

// Estrutura de usuário
typedef struct {
    char username[50];
    char password_hash[100];
    int is_admin;
} User;

// Autentica um usuário, retorna 1 se autenticado, 0 caso contrário
int authenticate_user();

// Verifica se o usuário atual tem permissão de administrador
int is_admin_user();

#endif
