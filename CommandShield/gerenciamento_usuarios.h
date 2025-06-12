#ifndef GERENCIAMENTO_USUARIOS_H
#define GERENCIAMENTO_USUARIOS_H

#include "sqlite_wrapper.h" // Inclua se necessário

// Estrutura do usuário (se não definida em outro lugar)
typedef struct {
    char username[50];
    int is_admin;
} User;

extern User current_user;
extern SQLiteConnection* sqlite_conn;

// Declarações das funções públicas
int authenticate_user();
int is_admin_user();
void listar_usuarios();
int alterar_senha(const char* login, const char* senha_nova);
// Adicione outras declarações se necessário

#endif // GERENCIAMENTO_USUARIOS_H

