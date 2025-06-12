#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite_wrapper.h"
#include "gerenciamento_usuarios.h"
#include "processador_comandos.h"
#include "logging.h"

// Variável global SQLite
SQLiteConnection* sqlite_conn = NULL;

// Caminho do banco
const char* db_path = "commandshield.db";

// Funções para inicializar banco e tabela 
void setup_database() {

    const char* create_users_table =
        "CREATE TABLE IF NOT EXISTS tbusuario ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "login VARCHAR(50) UNIQUE NOT NULL, "
        "senha CHAR(10) NOT NULL, " 
        "data_criacao DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (sqlite_execute_query(sqlite_conn, create_users_table) == 0) {
    }
    else {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro ao criar tabela: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
    }
}

int init_sqlite_connection() {
    sqlite_conn = sqlite_connect(db_path);
    if (sqlite_conn == NULL) {
        printf("Erro: Não foi possível criar/abrir banco SQLite: %s\n", db_path);
        return 0; 
    }

    // Testar a conexão 
    int test_result = sqlite_execute_query(sqlite_conn, "SELECT 1");
    if (test_result != 0) {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro de conexão SQLite após abertura: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
        sqlite_disconnect(sqlite_conn);
        sqlite_conn = NULL;
        return 0;
    }

    // Chama setup_database após conectar com sucesso
    setup_database();

    return 1; 
}

int main() {
    char command[256];
    int status = 1;

    printf("=== CommandShield - Sistema de Autenticação ===\n\n");

    init_system(); 

    // Tenta conectar ao banco de dados
    if (!init_sqlite_connection()) {
        printf("Aviso: Falha ao conectar ao banco de dados. Algumas funcionalidades podem estar indisponíveis.\n");
    }

    // Autenticação (agora lida com sqlite_conn podendo ser nulo)
    if (!authenticate_user()) {
        printf("Falha na autenticação ou usuário optou por não continuar. Encerrando o programa.\n");
        cleanup_system(); 
        return 1;
    }

    // Loop principal do programa
    printf("\nBem-vindo ao CommandShield! Digite 'help' para comandos ou 'exit' para sair.\n");
    while (status) {
        printf("%s@CommandShield> ", current_user.username);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\nEntrada inválida ou fim de stream. Encerrando.\n");
            status = 0;
            continue;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) continue; 

        status = process_command(command);

        log_command(command); 
    }

    cleanup_system();

    printf("Programa encerrado.\n");
    return 0;
}
