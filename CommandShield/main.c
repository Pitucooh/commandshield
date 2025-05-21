// main.c - Ponto de entrada principal do programa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processador_comandos.h"
#include "gerenciamento_usuarios.h"
#include "logging.h"
//#include <mysql/mysql.h>  // Biblioteca MySQL Connector/C
//
//// Variáveis globais para conexão MySQL
//MYSQL* conn;
//MYSQL_RES* res;
//MYSQL_ROW row;
//
//// Configuração do banco - ajuste com seus dados
//const char* server = "localhost";         // Servidor MySQL
//const char* user = "root";                 // Usuário MySQL
//const char* password = "sua_senha";        // Senha MySQL
//const char* database = "nome_do_banco";   // Nome do banco
//
//// Função para autenticação do usuário no banco
//int authenticate_user(MYSQL* conn) {
//    char username[50];
//    char pass_input[11];  // senha limitada a 10 caracteres + '\0'
//    char query[256];
//
//    printf("Username: ");
//    fgets(username, sizeof(username), stdin);
//    username[strcspn(username, "\n")] = 0;
//
//    printf("Password (10 caracteres): ");
//    fgets(pass_input, sizeof(pass_input), stdin);
//    pass_input[strcspn(pass_input, "\n")] = 0;
//
//    if (strlen(pass_input) != 10) {
//        printf("Erro: A senha deve ter exatamente 10 caracteres.\n");
//        return 0;
//    }
//
//    // Preparar query para buscar a senha do usuário no banco
//    snprintf(query, sizeof(query),
//        "SELECT senha FROM tbusuario WHERE login='%s'", username);
//
//    if (mysql_query(conn, query)) {
//        fprintf(stderr, "Erro na consulta: %s\n", mysql_error(conn));
//        return 0;
//    }
//
//    res = mysql_store_result(conn);
//    if (res == NULL) {
//        fprintf(stderr, "Erro ao obter resultado: %s\n", mysql_error(conn));
//        return 0;
//    }
//
//    row = mysql_fetch_row(res);
//    if (row != NULL) {
//        // Comparar senha do banco (row[0]) com senha digitada
//        if (strcmp(row[0], pass_input) == 0) {
//            printf("Usuário autenticado com sucesso!\n");
//            mysql_free_result(res);
//            return 1;
//        }
//        else {
//            printf("Senha incorreta.\n");
//        }
//    }
//    else {
//        printf("Usuário não encontrado.\n");
//    }
//
//    mysql_free_result(res);
//    return 0;
//}

int main() {
    char command[256];
    int status = 1;

    // Inicialização do sistema
    init_system();

    // Inicializar MySQL
    //conn = mysql_init(NULL);
    //if (conn == NULL) {
    //    fprintf(stderr, "mysql_init() falhou\n");
    //    return EXIT_FAILURE;
    //}

    // Conectar no banco com dados definidos acima
 /*   if (mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() falhou:\nErro %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }*/

    //printf("Conexão MySQL estabelecida!\n");

    //// Autenticar usuário antes de continuar
    //if (!authenticate_user(conn)) {
    //    printf("Falha na autenticação. Programa encerrado.\n");
    //    mysql_close(conn);
    //    return 1;
    //}

       // Autenticação de usuário
    if (!authenticate_user()) {
        printf("Falha na autenticação. Encerrando o programa.\n");
        return 1;
    }

    // Exibir o banner de boas-vindas após autenticação bem-sucedida
    display_welcome_banner();

    // Loop simples para comandos após autenticação
    while (status) {
        printf("CommandShield> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("Erro ao ler comando. Encerrando.\n");
            break;
        }

        command[strcspn(command, "\n")] = 0;

        // Ignorar comandos vazios
        if (command[0] == '\0') {
            continue;
        }

        // 🔽 Processar comando
        status = process_command(command);

        // 🔽 Registrar log (exceto para "history")
        if (strcmp(command, "history") != 0) {
            log_command(command);
        }
    }

    // Finalização do sistema
    cleanup_system();

    // Pausa opcional antes de encerrar (útil se estiver executando fora do VS)
    printf("\nPressione Enter para encerrar...");
    getchar();

    // Fechar conexão com MySQL
    //mysql_close(conn);
    printf("Conexão MySQL fechada.\n");

    return 0;
}
