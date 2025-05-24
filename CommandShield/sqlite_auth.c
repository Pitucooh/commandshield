// sqlite_auth.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite_wrapper.h"
#include "sqlite_auth.h"

void cesar_decifrar(const char* input, char* output);

static SQLiteConnection* conn = NULL;

bool sqlite_init(const char* db_path) {
    conn = sqlite_connect(db_path);
    if (!conn) {
        printf("Erro ao conectar SQLite\n");
        return false;
    }
    return true;
}

bool sqlite_setup_database(void) {
    const char* sql_create = "CREATE TABLE IF NOT EXISTS tbusuario ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE NOT NULL,"
        "senha TEXT NOT NULL,"
        "data_criacao DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    if (sqlite_execute_query(conn, sql_create) != 0) {
        printf("Erro ao criar tabela tbusuario\n");
        return false;
    }

    // Verificar se admin existe
    const char* check_admin = "SELECT COUNT(*) FROM tbusuario WHERE login='admin';";
    SQLiteResult* res = sqlite_select_query(conn, check_admin);
    if (res && res->rows > 0) {
        int count = atoi(res->data[0]);
        sqlite_free_result(res);
        if (count == 0) {
            // Inserir admin padrão
            const char* insert_admin = "INSERT INTO tbusuario (login, senha) VALUES ('admin', 'dgplq456');";
            if (sqlite_execute_query(conn, insert_admin) != 0) {
                printf("Erro ao inserir usuário admin\n");
                return false;
            }
            printf("Usuário admin criado (senha padrão: 'admin123')\n");
        }
    }
    else {
        printf("Erro ao verificar usuário admin\n");
        return false;
    }

    return true;
}

bool sqlite_authenticate_user(void) {
    char login[50];
    char senha[20];

    printf("Login: ");
    if (!fgets(login, sizeof(login), stdin)) return false;
    login[strcspn(login, "\n")] = 0;

    printf("Senha: ");
    if (!fgets(senha, sizeof(senha), stdin)) return false;
    senha[strcspn(senha, "\n")] = 0;

    char sql_query[256];
    snprintf(sql_query, sizeof(sql_query), "SELECT senha FROM tbusuario WHERE login='%s';", login);

    SQLiteResult* res = sqlite_select_query(conn, sql_query);
    if (!res || res->rows == 0) {
        printf("Usuário não encontrado\n");
        if (res) sqlite_free_result(res);
        return false;
    }

    // A senha está armazenada cifrada com César 3 (exemplo)
    char* senha_armazenada = res->data[0];
    sqlite_free_result(res);

    // Função para decifrar César 3 (implemente conforme sua lógica)
    char senha_decifrada[20];
    cesar_decifrar(senha_armazenada, senha_decifrada);

    if (strcmp(senha, senha_decifrada) == 0) {
        printf("Autenticado com sucesso!\n");
        return true;
    }
    else {
        printf("Senha incorreta.\n");
        return false;
    }
}

void sqlite_close(void) {
    if (conn) {
        sqlite_disconnect(conn);
        conn = NULL;
    }
}

// Função de exemplo para decifrar César 3
void cesar_decifrar(const char* input, char* output) {
    int i = 0;
    while (input[i]) {
        char c = input[i];
        if (c >= 'a' && c <= 'z') {
            c = ((c - 'a' - 3 + 26) % 26) + 'a';
        }
        else if (c >= 'A' && c <= 'Z') {
            c = ((c - 'A' - 3 + 26) % 26) + 'A';
        }
        output[i] = c;
        i++;
    }
    output[i] = '\0';
}
