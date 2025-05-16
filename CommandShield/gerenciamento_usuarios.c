#include "gerenciamento_usuarios.h"
#include "ferramentas_cripto.h"  // sua cifra de César
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql>

User current_user;

MYSQL* conectar_banco() {
    MYSQL* conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "localhost", "root", "sua_senha", "seubanco", 0, NULL, 0)) {
        fprintf(stderr, "Erro de conexão: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

int login_existe(MYSQL* conn, const char* login) {
    char query[200];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM tbusuario WHERE login = '%s'", login);
    if (mysql_query(conn, query) != 0) return 0;

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    int count = atoi(row[0]);
    mysql_free_result(res);
    return count > 0;
}

int verificar_senha(MYSQL* conn, const char* login, const char* senha_plain) {
    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3); // exemplo com chave 3

    char query[300];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM tbusuario WHERE login = '%s' AND senha = '%s'",
        login, senha_cript);

    if (mysql_query(conn, query) != 0) return 0;

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    int ok = atoi(row[0]);
    mysql_free_result(res);
    return ok > 0;
}

void insert_user(MYSQL* conn, const char* login, const char* senha_plain) {
    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3); // mesma chave usada para criptografar

    char query[300];
    snprintf(query, sizeof(query),
        "INSERT INTO tbusuario (login, senha) VALUES ('%s', '%s')",
        login, senha_cript);

    if (mysql_query(conn, query) != 0) {
        printf("Erro ao inserir usuário: %s\n", mysql_error(conn));
    }
    else {
        printf("Usuário cadastrado com sucesso!\n");
    }
}

int authenticate_user() {
    char login[100];
    char senha[100];

    printf("Informe seu login: ");
    fgets(login, sizeof(login), stdin);
    login[strcspn(login, "\n")] = 0;

    MYSQL* conn = conectar_banco();
    if (!conn) return 0;

    if (login_existe(conn, login)) {
        printf("Login encontrado. Digite sua senha: ");
        fgets(senha, sizeof(senha), stdin);
        senha[strcspn(senha, "\n")] = 0;

        if (strlen(senha) > 10) {
            senha[10] = '\0';
            printf("A senha será truncada para os 10 primeiros caracteres.\n");
        }

        if (verificar_senha(conn, login, senha)) {
            strcpy(current_user.username, login);
            mysql_close(conn);
            return 1;
        }
        else {
            printf("Senha incorreta!\n");
            mysql_close(conn);
            return 0;
        }

    }
    else {
        printf("Login não encontrado. Deseja cadastrar? (s/n): ");
        char opcao = getchar();
        getchar(); // consome o '\n'

        if (opcao == 's' || opcao == 'S') {
            printf("Digite uma senha de até 10 caracteres: ");
            fgets(senha, sizeof(senha), stdin);
            senha[strcspn(senha, "\n")] = 0;

            if (strlen(senha) > 10) {
                senha[10] = '\0';
                printf("Senha muito longa! Apenas os 10 primeiros caracteres serão usados.\n");
            }

            insert_user(conn, login, senha);
            strcpy(current_user.username, login);
            mysql_close(conn);
            return 1;
        }
        else {
            printf("Você entrou como convidado (sem login salvo).\n");
            strcpy(current_user.username, "convidado");
            mysql_close(conn);
            return 1;
        }
    }
}

int is_admin_user() {
    return strcmp(current_user.username, "admin") == 0;
}
