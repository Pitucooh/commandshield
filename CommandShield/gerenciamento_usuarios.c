#include "gerenciamento_usuarios.h"
#include "ferramentas_cripto.h"  // sua cifra de César
#include "mysql_wrapper.h"       // Nova wrapper MySQL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

User current_user;

// Variável externa para conexão MySQL (definida no main.c)
extern MySQLConnection* mysql_conn;

// Função para verificar se login existe no banco
int login_existe_mysql(const char* login) {
    if (!mysql_conn) return 0;

    char query[200];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM tbusuario WHERE login = '%s'", login);

    QueryResult* result = mysql_select_query(mysql_conn, query);
    if (!result || result->rows == 0) {
        if (result) mysql_free_result(result);
        return 0;
    }

    int count = atoi(result->data[0]);
    mysql_free_result(result);
    return count > 0;
}

// Função para verificar senha no banco com criptografia
int verificar_senha_mysql(const char* login, const char* senha_plain) {
    if (!mysql_conn) return 0;

    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3); // usando sua cifra de César com chave 3

    char query[300];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM tbusuario WHERE login = '%s' AND senha = '%s'",
        login, senha_cript);

    QueryResult* result = mysql_select_query(mysql_conn, query);
    if (!result || result->rows == 0) {
        if (result) mysql_free_result(result);
        return 0;
    }

    int ok = atoi(result->data[0]);
    mysql_free_result(result);
    return ok > 0;
}

// Função para inserir novo usuário no banco
void insert_user_mysql(const char* login, const char* senha_plain) {
    if (!mysql_conn) {
        printf("Erro: Conexão MySQL não disponível\n");
        return;
    }

    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3); // mesma chave usada para criptografar

    char query[300];
    snprintf(query, sizeof(query),
        "INSERT INTO tbusuario (login, senha) VALUES ('%s', '%s')",
        login, senha_cript);

    if (mysql_execute_query(mysql_conn, query) == 0) {
        printf("Usuário cadastrado com sucesso!\n");
    }
    else {
        char* error = mysql_get_last_error(mysql_conn);
        printf("Erro ao inserir usuário: %s\n", error);
        free(error);
    }
}

// Função de autenticação local (fallback)
int authenticate_user_local() {
    char username[50];
    char password[50];

    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) != NULL) {
        username[strcspn(username, "\n")] = 0;
    }

    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) != NULL) {
        password[strcspn(password, "\n")] = 0;
    }

    // Verificar credenciais hardcoded (para fallback)
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        strcpy_s(current_user.username, sizeof(current_user.username), username);
        current_user.is_admin = 1;
        return 1;
    }
    else if (strcmp(username, "user") == 0 && strcmp(password, "user123") == 0) {
        strcpy_s(current_user.username, sizeof(current_user.username), username);
        current_user.is_admin = 0;
        return 1;
    }

    return 0;
}

// Função de autenticação MySQL (principal)
int authenticate_user_mysql_full() {
    char login[100];
    char senha[100];

    printf("Informe seu login: ");
    if (fgets(login, sizeof(login), stdin) != NULL) {
        login[strcspn(login, "\n")] = 0;
    }

    if (!mysql_conn) {
        printf("Conexão MySQL não disponível. Usando autenticação local.\n");
        return authenticate_user_local();
    }

    if (login_existe_mysql(login)) {
        printf("Login encontrado. Digite sua senha: ");
        if (fgets(senha, sizeof(senha), stdin) != NULL) {
            senha[strcspn(senha, "\n")] = 0;
        }

        if (strlen(senha) > 10) {
            senha[10] = '\0';
            printf("A senha será truncada para os 10 primeiros caracteres.\n");
        }

        if (verificar_senha_mysql(login, senha)) {
            strcpy_s(current_user.username, sizeof(current_user.username), login);
            // Verificar se é admin (você pode adicionar um campo is_admin na tabela)
            current_user.is_admin = (strcmp(login, "admin") == 0);
            printf("Login realizado com sucesso!\n");
            return 1;
        }
        else {
            printf("Senha incorreta!\n");
            return 0;
        }
    }
    else {
        printf("Login não encontrado. Deseja cadastrar? (s/n): ");
        char opcao = getchar();
        getchar(); // consome o '\n'

        if (opcao == 's' || opcao == 'S') {
            printf("Digite uma senha de até 10 caracteres: ");
            if (fgets(senha, sizeof(senha), stdin) != NULL) {
                senha[strcspn(senha, "\n")] = 0;
            }

            if (strlen(senha) > 10) {
                senha[10] = '\0';
                printf("Senha muito longa! Apenas os 10 primeiros caracteres serão usados.\n");
            }

            insert_user_mysql(login, senha);
            strcpy_s(current_user.username, sizeof(current_user.username), login);
            current_user.is_admin = 0; // Novos usuários não são admin por padrão
            return 1;
        }
        else {
            printf("Você entrou como convidado (sem login salvo).\n");
            strcpy_s(current_user.username, sizeof(current_user.username), "convidado");
            current_user.is_admin = 0;
            return 1;
        }
    }
}

// Função principal de autenticação (usada no main)
int authenticate_user() {
    // Se MySQL está disponível, usa autenticação completa
    if (mysql_conn) {
        return authenticate_user_mysql_full();
    }
    // Senão, usa autenticação local
    else {
        printf("Modo offline - Autenticação local\n");
        return authenticate_user_local();
    }
}

// Verificar se usuário é admin
int is_admin_user() {
    return current_user.is_admin;
}

// Função para listar usuários (nova funcionalidade)
void listar_usuarios() {
    if (!mysql_conn) {
        printf("Conexão MySQL não disponível.\n");
        return;
    }

    if (!is_admin_user()) {
        printf("Acesso negado. Apenas administradores podem listar usuários.\n");
        return;
    }

    const char* query = "SELECT login, data_criacao FROM tbusuario ORDER BY data_criacao DESC";
    QueryResult* result = mysql_select_query(mysql_conn, query);

    if (!result) {
        char* error = mysql_get_last_error(mysql_conn);
        printf("Erro ao consultar usuários: %s\n", error);
        free(error);
        return;
    }

    printf("\n=== Lista de Usuários ===\n");
    printf("%-20s | %s\n", "Login", "Data de Criação");
    printf("----------------------------------------\n");

    for (int i = 0; i < result->rows; i++) {
        printf("%-20s | %s\n",
            result->data[i * result->cols + 0],  // login
            result->data[i * result->cols + 1]); // data_criacao
    }

    printf("\nTotal: %d usuários\n", result->rows);
    mysql_free_result(result);
}

// Função para alterar senha (nova funcionalidade)
int alterar_senha(const char* login, const char* senha_nova) {
    if (!mysql_conn) {
        printf("Conexão MySQL não disponível.\n");
        return 0;
    }

    // Verificar se é o próprio usuário ou admin
    if (strcmp(current_user.username, login) != 0 && !is_admin_user()) {
        printf("Acesso negado. Você só pode alterar sua própria senha.\n");
        return 0;
    }

    if (strlen(senha_nova) > 10) {
        printf("Erro: Senha deve ter no máximo 10 caracteres.\n");
        return 0;
    }

    char senha_cript[256];
    encrypt_string(senha_nova, senha_cript, 3);

    char query[300];
    snprintf(query, sizeof(query),
        "UPDATE tbusuario SET senha = '%s' WHERE login = '%s'",
        senha_cript, login);

    if (mysql_execute_query(mysql_conn, query) == 0) {
        printf("Senha alterada com sucesso!\n");
        return 1;
    }
    else {
        char* error = mysql_get_last_error(mysql_conn);
        printf("Erro ao alterar senha: %s\n", error);
        free(error);
        return 0;
    }
}