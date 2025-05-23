#ifndef GERENCIAMENTO_USUARIOS_H
#define GERENCIAMENTO_USUARIOS_H

// Estrutura para usuário atual
typedef struct {
    char username[100];
    int is_admin;
} User;

// Variável global do usuário atual
extern User current_user;

// Funções principais de autenticação
int authenticate_user();                    // Função principal (auto-detecta MySQL/local)
int authenticate_user_local();              // Autenticação local (fallback)
int authenticate_user_mysql_full();         // Autenticação MySQL completa
int is_admin_user();                        // Verificar se é admin

// Funções MySQL específicas
int login_existe_mysql(const char* login);
int verificar_senha_mysql(const char* login, const char* senha_plain);
void insert_user_mysql(const char* login, const char* senha_plain);

// Funções de gerenciamento (novas)
void listar_usuarios();                     // Listar usuários (admin only)
int alterar_senha(const char* login, const char* senha_nova);  // Alterar senha

#endif // GERENCIAMENTO_USUARIOS_H