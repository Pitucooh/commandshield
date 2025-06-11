#include "gerenciamento_usuarios.h"
#include "ferramentas_cripto.h"  
#include "sqlite_wrapper.h"     
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _MSC_VER
#include <stddef.h>
#endif

User current_user; 

// Variável externa para conexão SQLite (definida no main.c)
extern SQLiteConnection* sqlite_conn;

// Função para verificar se login existe no banco
int login_existe_sqlite(const char* login) {
    if (!sqlite_conn) return 0;
    char query[200];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM tbusuario WHERE login = '%s'", login);
    SQLiteResult* result = sqlite_select_query(sqlite_conn, query);
    if (!result || result->rows == 0) {
        if (result) sqlite_free_result(result);
        return 0;
    }
    int count = atoi(result->data[0]);
    sqlite_free_result(result);
    return count > 0;
}

// Função para verificar senha no banco com criptografia
int verificar_senha_sqlite(const char* login, const char* senha_plain) {
    if (!sqlite_conn) return 0;
    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3);

    char query[300];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM tbusuario WHERE login = '%s' AND senha = '%s'",
        login, senha_cript);
    SQLiteResult* result = sqlite_select_query(sqlite_conn, query);
    if (!result || result->rows == 0) {
        if (result) sqlite_free_result(result);
        return 0;
    }
    int ok = atoi(result->data[0]);
    sqlite_free_result(result);
    return ok > 0;
}

// Função para inserir novo usuário no banco
int insert_user_sqlite(const char* login, const char* senha_plain) {
    if (!sqlite_conn) {
        printf("Erro: Conexão SQLite não disponível para inserir usuário.\n");
        return 0;
    }

    char senha_cript[256];
    encrypt_string(senha_plain, senha_cript, 3);

    char query[300];
    snprintf(query, sizeof(query),
        "INSERT INTO tbusuario (login, senha) VALUES (	'%s', 	'%s')",
        login, senha_cript);
    if (sqlite_execute_query(sqlite_conn, query) == 0) {
        return 1;
    }
    else {
        char* error = sqlite_get_last_error(sqlite_conn);
        if (error && strstr(error, "UNIQUE constraint failed")) {
            printf("Erro: O login '%s' já existe.\n", login);
        }
        else {
            printf("Erro ao inserir usuário: %s\n", error ? error : "Erro desconhecido");
        }
        if (error) free(error);
        return 0;
    }
}

// Função de autenticação local (fallback, caso DB não conecte)
int authenticate_user_local() {
    printf("Aviso: Operando em modo offline (sem banco de dados).\n");
    printf("Entrando como convidado.\n");
#ifdef _MSC_VER
    strncpy_s(current_user.username, sizeof(current_user.username), "convidado_offline", _TRUNCATE);
#else
    strncpy(current_user.username, "convidado_offline", sizeof(current_user.username) - 1);
    current_user.username[sizeof(current_user.username) - 1] = '\0';
#endif
    current_user.is_admin = 0;
    return 1;
}

// Função para alterar a própria senha ou a de outro usuário (se admin)
int alterar_senha(const char* login_alvo, const char* senha_nova) {
    if (!sqlite_conn) {
        printf("Funcionalidade indisponível em modo offline.\n");
        return 0;
    }

    // Valida o tamanho da nova senha
    if (strlen(senha_nova) != 8) {
        printf("Erro: A nova senha deve ter exatamente 8 caracteres.\n");
        return 0;
    }

    // Verifica se o login alvo realmente existe antes de tentar alterar
    if (!login_existe_sqlite(login_alvo)) {
        printf("Erro interno: Tentativa de alterar senha para login inexistente '%s'.\n", login_alvo);
        return 0;
    }

    char senha_cript[256];
    encrypt_string(senha_nova, senha_cript, 3);

    char query[300];
    snprintf(query, sizeof(query),
        "UPDATE tbusuario SET senha = '%s' WHERE login = '%s'",
        senha_cript, login_alvo);

    if (sqlite_execute_query(sqlite_conn, query) == 0) {
        int changes = sqlite_get_changes(sqlite_conn);
        if (changes > 0) {
            return 1;
        }
        else {
            printf("Nenhuma senha alterada para o login '%s'. A nova senha pode ser igual à anterior.\n", login_alvo);
            return 0;
        }
    }
    else {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro ao alterar senha no banco: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
        return 0;
    }
}


// Redefinição de Senha
int authenticate_user_sqlite_full() {
    char login[100];
    char senha[100];
    char login_confirm[100]; 
    int tentativas = 0;
    const int max_tentativas = 2;

    printf("Informe seu login: ");
    if (fgets(login, sizeof(login), stdin) != NULL) {
        login[strcspn(login, "\n")] = 0;
    }
    else {
        printf("Erro ao ler login.\n");
        return 0;
    }

    if (strlen(login) == 0) {
        printf("Login não pode ser vazio.\n");
        return 0;
    }

    if (login_existe_sqlite(login)) {
        // Login existente 
        while (tentativas < max_tentativas) {
            tentativas++;
            printf("Login '%s' encontrado. Digite sua senha (%d/%d): ", login, tentativas, max_tentativas);
            if (fgets(senha, sizeof(senha), stdin) != NULL) {
                senha[strcspn(senha, "\n")] = 0;
            }
            else {
                printf("Erro ao ler senha.\n");
                return 0;
            }

            if (verificar_senha_sqlite(login, senha)) {
                // Senha correta
#ifdef _MSC_VER
                strncpy_s(current_user.username, sizeof(current_user.username), login, _TRUNCATE);
#else
                strncpy(current_user.username, login, sizeof(current_user.username) - 1);
                current_user.username[sizeof(current_user.username) - 1] = '\0';
#endif
                current_user.is_admin = (strcmp(login, "admin") == 0);
                printf("Login realizado com sucesso! Bem-vindo, %s.\n", current_user.username);
                return 1;
            }
            else {
                printf("Senha incorreta!%s\n", (tentativas < max_tentativas) ? " Tente novamente." : "");
            }
        }

        // Falha após tentativas  
        printf("Número máximo de tentativas de senha excedido para o login '%s'.\n", login);
        printf("Deseja redefinir sua senha? (s/n): ");
        char opcao_reset = getchar();
        int c;
        while ((c = getchar()) != '\n' && c != EOF); // Limpa buffer

        if (opcao_reset == 's' || opcao_reset == 'S') {
            // Fluxo de Redefinição 
            printf("Para confirmar, digite seu login novamente: ");
            if (fgets(login_confirm, sizeof(login_confirm), stdin) != NULL) {
                login_confirm[strcspn(login_confirm, "\n")] = 0;
            }
            else {
                printf("Erro ao ler confirmação de login.\n");
                return 0;
            }

            // Verifica se o login confirmado é o mesmo do início
            if (strcmp(login, login_confirm) != 0) {
                printf("Login de confirmação não corresponde ao login inicial (	'%s'). Redefinição cancelada.\n", login);
                return 0;
            }

            // Loop para garantir senha de exatamente 8 caracteres
            while (1) {
                printf("Digite uma nova senha de EXATAMENTE 8 caracteres para o login '%s': ", login);
                if (fgets(senha, sizeof(senha), stdin) != NULL) {
                    senha[strcspn(senha, "\n")] = 0;
                }
                else {
                    printf("Erro ao ler nova senha.\n");
                    return 0;
                }

                if (strlen(senha) == 8) {
                    break;
                }
                else {
                    printf("Erro: A senha deve ter exatamente 8 caracteres. Tente novamente.\n");
                }
            }


            // Tenta alterar a senha no banco
            if (alterar_senha(login, senha)) {
                // Sucesso na alteração: Loga o usuário
#ifdef _MSC_VER
                strncpy_s(current_user.username, sizeof(current_user.username), login, _TRUNCATE);
#else
                strncpy(current_user.username, login, sizeof(current_user.username) - 1);
                current_user.username[sizeof(current_user.username) - 1] = '\0';
#endif
                current_user.is_admin = (strcmp(login, "admin") == 0);
                printf("Senha redefinida com sucesso! Login realizado para '%s'.\n", login);
                return 1; 
            }
            else {
                printf("Não foi possível redefinir a senha. Tente novamente mais tarde.\n");
                return 0; 
            }
        }
        else {
            printf("Você optou por não redefinir a senha.\n");
            return 0; 
        }
    }
    else {
        //Login Inexistente
        printf("Login 	'%s' não encontrado. Deseja cadastrar este login? (s/n): ", login);
        char opcao_cad = getchar();
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (opcao_cad == 's' || opcao_cad == 'S') {
            // Cadastro de Novo Usuário 
            printf("Digite uma senha de EXATAMENTE 8 caracteres para o login '%s': ", login);
            if (fgets(senha, sizeof(senha), stdin) != NULL) {
                senha[strcspn(senha, "\n")] = 0;
            }
            else {
                printf("Erro ao ler senha para cadastro.\n");
                return 0;
            }

            if (strlen(senha) != 8) {
                printf("Erro: A senha deve ter exatamente 8 caracteres.\n");
                return 0;
            }

            if (insert_user_sqlite(login, senha)) {
#ifdef _MSC_VER
                strncpy_s(current_user.username, sizeof(current_user.username), login, _TRUNCATE);
#else
                strncpy(current_user.username, login, sizeof(current_user.username) - 1);
                current_user.username[sizeof(current_user.username) - 1] = '\0';
#endif
                current_user.is_admin = 0;
                printf("Usuário '%s' cadastrado com sucesso! Bem-vindo.\n", login);
                return 1;
            }
            else {
                printf("Não foi possível cadastrar o usuário.\n");
                return 0;
            }
        }
        else {
            // Acesso como convidado 
            printf("Você optou por não cadastrar. Usando o sistema como convidado com o login temporário 	'%s'.\n", login);
#ifdef _MSC_VER
            strncpy_s(current_user.username, sizeof(current_user.username), login, _TRUNCATE);
#else
            strncpy(current_user.username, login, sizeof(current_user.username) - 1);
            current_user.username[sizeof(current_user.username) - 1] = '\0';
#endif
            current_user.is_admin = 0;
            return 1;
        }
    }
}

// Função principal de autenticação (chamada pelo main)
int authenticate_user() {
    srand((unsigned int)time(NULL));
    if (!sqlite_conn) {
        return authenticate_user_local();
    }
    else {
        return authenticate_user_sqlite_full();
    }
}

// Verificar se usuário logado é admin
int is_admin_user() {
    return current_user.is_admin;
}

// Função para listar usuários (requer admin)
void listar_usuarios() {
    if (!sqlite_conn) {
        printf("Funcionalidade indisponível em modo offline.\n");
        return;
    }
    if (!is_admin_user()) {
        printf("Acesso negado. Apenas administradores podem listar usuários.\n");
        return;
    }
    const char* query = "SELECT login, data_criacao FROM tbusuario ORDER BY data_criacao DESC";
    SQLiteResult* result = sqlite_select_query(sqlite_conn, query);
    if (!result) {
        char* error = sqlite_get_last_error(sqlite_conn);
        printf("Erro ao consultar usuários: %s\n", error ? error : "Erro desconhecido");
        if (error) free(error);
        return;
    }
    printf("\n=== Lista de Usuários Cadastrados ===\n");
    printf("%-20s | %s\n", "Login", "Data de Criação");
    printf("----------------------------------------\n");
    if (result->rows == 0) {
        printf("(Nenhum usuário cadastrado)\n");
    }
    else {
        for (int i = 0; i < result->rows; i++) {
            printf("%-20s | %s\n",
                result->data[i * result->cols + 0],
                result->data[i * result->cols + 1]);
        }
    }
    printf("\nTotal: %d usuários cadastrados\n", result->rows);
    sqlite_free_result(result);
}