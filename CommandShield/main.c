#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite_wrapper.h"
#include "gerenciamento_usuarios.h"
#include "processador_comandos.h"
#include "logging.h"

// Variável global da conexão SQLite
SQLiteConnection* sqlite_conn = NULL;

// Caminho do banco
const char* db_path = "commandshield.db";

// --- Funções para inicializar banco e tabela ---
void setup_database() {

    const char* create_users_table =
        "CREATE TABLE IF NOT EXISTS tbusuario ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "login VARCHAR(50) UNIQUE NOT NULL, "
        "senha CHAR(10) NOT NULL, " // Mantido CHAR(10) por compatibilidade, mas lógica usará 8
        "data_criacao DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (sqlite_execute_query(sqlite_conn, create_users_table) == 0) {
    }
    else {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro ao criar tabela: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
        // Considerar se deve sair ou tentar continuar sem o banco
        // exit(1); // Comentado para permitir talvez modo offline
    }

    // *** REMOVIDO: Bloco que criava o usuário admin padrão ***
    // Não haverá mais usuário padrão criado automaticamente.
}

int init_sqlite_connection() {
    sqlite_conn = sqlite_connect(db_path);
    if (sqlite_conn == NULL) {
        printf("Erro: Não foi possível criar/abrir banco SQLite: %s\n", db_path);
        // Tentar continuar em modo offline? Ou retornar erro?
        return 0; // Retorna 0 indicando falha na conexão
    }

    // Testar a conexão (opcional, mas bom)
    int test_result = sqlite_execute_query(sqlite_conn, "SELECT 1");
    if (test_result != 0) {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro de conexão SQLite após abertura: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
        sqlite_disconnect(sqlite_conn);
        sqlite_conn = NULL;
        return 0; // Falha
    }

    // Chama setup_database APÓS conectar com sucesso
    setup_database();

    return 1; // Sucesso
}

// REMOVIDA a implementação duplicada de cleanup_system
// A função está implementada em processador_comandos.c

int main() {
    char command[256];
    int status = 1;

    printf("=== CommandShield - Sistema de Autenticação ===\n\n");

    // Inicializa sistema (ex: banner)
    init_system(); // Supondo que init_system() não dependa do DB

    // Tenta conectar ao banco de dados
    if (!init_sqlite_connection()) {
        printf("Aviso: Falha ao conectar ao banco de dados. Algumas funcionalidades podem estar indisponíveis.\n");
        // O programa pode continuar em modo offline ou com funcionalidade limitada
        // A variável sqlite_conn será NULL, e as funções de autenticação/gerenciamento devem tratar isso.
    }

    // Autenticação (agora lida com sqlite_conn podendo ser NULL)
    if (!authenticate_user()) {
        printf("Falha na autenticação ou usuário optou por não continuar. Encerrando o programa.\n");
        cleanup_system(); // Garante que a desconexão (se houve conexão) ocorra
        return 1;
    }

    // Loop principal do programa
    printf("\nBem-vindo ao CommandShield! Digite 'help' para comandos ou 'exit' para sair.\n");
    while (status) {
        // Exibe o prompt com o nome do usuário logado
        printf("%s@CommandShield> ", current_user.username);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Tratar fim de arquivo (Ctrl+D) ou erro de leitura
            printf("\nEntrada inválida ou fim de stream. Encerrando.\n");
            status = 0; // Sai do loop
            continue;
        }

        // Remover quebra de linha
        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) continue; // Ignora linha vazia

        // Processar comando (implementado em processador_comandos.c)
        status = process_command(command);

        // Registrar log do comando (em logging.c) - se log depende de usuário, passar current_user?
        // log_command(current_user.username, command); // Exemplo
        log_command(command); // Mantendo como estava
    }

    // Finaliza sistema (fecha conexões, libera recursos)
    cleanup_system();

    printf("Programa encerrado.\n");
    return 0;
}
