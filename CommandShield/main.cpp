// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_wrapper.h"  // sua wrapper MySQL em C++
#include <mysqlx/xdevapi.h>

MySQLConnection* mysql_conn = NULL;

// Configuração do banco - ajuste com seus dados
const char* server = "localhost";
const char* user = "root";
const char* password = "sua_senha";  // Atualize aqui
const char* database = "commandshield_db";
const int port = 3306;  // Porta padrão MySQL (ajuste se precisar)


// Função para criar tabela tbusuario se não existir
void setup_database() {
    printf("Verificando/criando tabela tbusuario...\n");

    const char* create_users_table =
        "CREATE TABLE IF NOT EXISTS tbusuario ("
        "id INT AUTO_INCREMENT PRIMARY KEY, "
        "login VARCHAR(50) UNIQUE NOT NULL, "
        "senha CHAR(10) NOT NULL, "
        "data_criacao TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (mysql_execute_query(mysql_conn, create_users_table) == 0) {
        printf("✅ Tabela tbusuario pronta\n");
    }
    else {
        char* error = mysql_get_last_error(mysql_conn);
        printf("Erro ao criar tabela: %s\n", error);
        free(error);
        exit(1);
    }
}


// Função para verificar se usuário existe
int user_exists(const char* login) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT login FROM tbusuario WHERE login='%s'", login);
    QueryResult* result = mysql_select_query(mysql_conn, query);
    if (!result) {
        char* err = mysql_get_last_error(mysql_conn);
        printf("Erro SQL: %s\n", err);
        free(err);
        return 0;
    }
    int exists = (result->rows > 0);
    mysql_free_result(result);
    return exists;
}

// Função que valida a senha do usuário
int validate_password(const char* login, const char* senha_input) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT senha FROM tbusuario WHERE login='%s'", login);
    QueryResult* result = mysql_select_query(mysql_conn, query);
    if (!result) {
        char* err = mysql_get_last_error(mysql_conn);
        printf("Erro SQL: %s\n", err);
        free(err);
        return 0;
    }
    if (result->rows == 0) {
        mysql_free_result(result);
        return 0; // usuário não existe
    }

    char* senha_banco = result->data[0];
    int is_valid = (strcmp(senha_banco, senha_input) == 0);
    mysql_free_result(result);
    return is_valid;
}

// Função para cadastrar novo usuário
int register_user(const char* login) {
    char senha[46];
    while (1) {
        printf("Digite a senha para o novo usuário (até 45 caracteres): ");
        if (fgets(senha, sizeof(senha), stdin) == NULL) {
            printf("Erro ao ler senha.\n");
            return 0;
        }
        senha[strcspn(senha, "\n")] = 0;

        int len = strlen(senha);
        if (len == 0 || len > 45) {
            printf("Senha inválida. Deve ter entre 1 e 45 caracteres.\n");
            continue;
        }
        break;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "INSERT INTO tbusuario (login, senha) VALUES ('%s', '%s')",
        login, senha);

    int ret = mysql_execute_query(mysql_conn, query);
    if (ret != 0) {
        char* err = mysql_get_last_error(mysql_conn);
        printf("Erro ao cadastrar usuário: %s\n", err);
        free(err);
        return 0;
    }
    printf("Usuário '%s' cadastrado com sucesso!\n", login);
    return 1;
}

// Função principal de autenticação com fluxo completo:
int authenticate_user_mysql() {
    char login[46];

    while (1) {
        printf("Login: ");
        if (fgets(login, sizeof(login), stdin) == NULL) {
            printf("Erro ao ler login.\n");
            return 0;
        }
        login[strcspn(login, "\n")] = 0;

        if (strlen(login) == 0) {
            printf("Login não pode ser vazio.\n");
            continue;
        }

        if (user_exists(login)) {
            // Usuário existe, pedir senha e validar
            char senha_input[46];
            while (1) {
                printf("Senha (até 45 caracteres): ");
                if (fgets(senha_input, sizeof(senha_input), stdin) == NULL) {
                    printf("Erro ao ler senha.\n");
                    return 0;
                }
                senha_input[strcspn(senha_input, "\n")] = 0;

                int len = strlen(senha_input);
                if (len == 0 || len > 45) {
                    printf("Senha inválida. Deve ter entre 1 e 45 caracteres.\n");
                    continue;
                }

                if (validate_password(login, senha_input)) {
                    printf("Usuário autenticado com sucesso!\n");
                    return 1;
                }
                else {
                    printf("Senha incorreta. Tente novamente.\n");
                }
            }
        }
        else {
            // Usuário novo, perguntar se quer cadastrar
            printf("Usuário '%s' não encontrado. Deseja cadastrar senha? (s/n): ", login);
            char resposta = getchar();
            while (getchar() != '\n'); // limpar buffer

            if (resposta == 's' || resposta == 'S') {
                if (register_user(login)) {
                    return 1;
                }
                else {
                    printf("Falha no cadastro. Tente novamente.\n");
                }
            }
            else {
                printf("Usuário não cadastrado, mas acesso liberado temporariamente.\n");
                return 1;
            }
        }
    }
}

// Função para inicializar conexão MySQL
int init_mysql_connection() {
    printf("Estabelecendo conexão com MySQL...\n");

    mysql_conn = mysql_connect(server, port, user, password, database);

    if (mysql_conn == NULL) {
        printf("Erro: Não foi possível criar conexão MySQL\n");
        return 0;
    }

    // Testar conexão
    int test_result = mysql_execute_query(mysql_conn, "SELECT 1");
    if (test_result != 0) {
        char* error = mysql_get_last_error(mysql_conn);
        printf("Erro de conexão MySQL: %s\n", error);
        free(error);
        mysql_disconnect(mysql_conn);
        mysql_conn = NULL;
        return 0;
    }

    printf("Conexão MySQL estabelecida com sucesso!\n");
    return 1;
}

int main() {
    // Inicializa sistema, conecta no banco e configura tabela
    if (!init_mysql_connection()) {
        printf("Falha na conexão MySQL. Encerrando.\n");
        return 1;
    }
    setup_database();

    // Autenticação
    if (!authenticate_user_mysql()) {
        printf("Falha na autenticação. Encerrando.\n");
        mysql_disconnect(mysql_conn);
        return 1;
    }

    // Aqui segue seu sistema depois da autenticação
    printf("Bem-vindo ao sistema!\n");

    // Fechar conexão no final
    mysql_disconnect(mysql_conn);
    printf("Conexão MySQL fechada.\n");
    return 0;
}
