#include "sqlite_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Incluir stddef.h para _TRUNCATE se não estiver incluído por outros headers no MSVC
#ifdef _MSC_VER
#include <stddef.h>
// Incluir definição de strcpy_s se não estiver em string.h por padrão
#if !defined(__STDC_LIB_EXT1__)
#include <errno.h>
// Definição simples de strcpy_s para compatibilidade básica se não existir
static inline errno_t strcpy_s(char* dest, size_t destsz, const char* src) {
    if (dest == NULL || destsz == 0 || src == NULL) return EINVAL;
    size_t len = strlen(src);
    if (len >= destsz) {
        dest[0] = '\0'; // Truncar
        return STRUNCATE;
    }
    memcpy(dest, src, len + 1);
    return 0;
}
#endif
#endif

// Conecta ao banco SQLite
SQLiteConnection* sqlite_connect(const char* db_path) {
    SQLiteConnection* conn = malloc(sizeof(SQLiteConnection));
    if (!conn) return NULL;

    conn->last_error = NULL;

    int rc = sqlite3_open(db_path, &conn->db);
    if (rc != SQLITE_OK) {
        const char* error_msg = sqlite3_errmsg(conn->db);
        size_t len = strlen(error_msg) + 1;
        conn->last_error = malloc(len);
        if (conn->last_error) {
#ifdef _MSC_VER
            strcpy_s(conn->last_error, len, error_msg);
#else
            strncpy(conn->last_error, error_msg, len - 1);
            conn->last_error[len - 1] = '\0';
#endif
        }
        sqlite3_close(conn->db); // Fecha mesmo em caso de erro de abertura
        // free(conn->last_error); // Não liberar aqui, pois é retornado implicitamente
        // free(conn); // Não liberar conn aqui, pois o ponteiro NULL será retornado
        // Correção: Liberar conn se last_error foi alocado, senão vaza memória
        if (conn->last_error == NULL) free(conn);
        return NULL; // Retorna NULL em caso de erro
    }

    return conn;
}

// Desconecta do banco
void sqlite_disconnect(SQLiteConnection* conn) {
    if (!conn) return;

    if (conn->db) {
        sqlite3_close(conn->db);
    }

    if (conn->last_error) {
        free(conn->last_error);
    }

    free(conn);
}

// Executa query sem retorno (INSERT, UPDATE, DELETE, CREATE)
int sqlite_execute_query(SQLiteConnection* conn, const char* query) {
    if (!conn || !conn->db) return -1;

    // Limpar erro anterior
    if (conn->last_error) {
        free(conn->last_error);
        conn->last_error = NULL;
    }

    char* error_msg = NULL;
    int rc = sqlite3_exec(conn->db, query, NULL, NULL, &error_msg);

    if (rc != SQLITE_OK) {
        if (error_msg) {
            size_t len = strlen(error_msg) + 1;
            conn->last_error = malloc(len);
            if (conn->last_error) {
#ifdef _MSC_VER
                strcpy_s(conn->last_error, len, error_msg);
#else
                strncpy(conn->last_error, error_msg, len - 1);
                conn->last_error[len - 1] = '\0';
#endif
            }
            sqlite3_free(error_msg);
        }
        return -1;
    }

    return 0;
}

// Callback para sqlite3_exec nas consultas SELECT
static int select_callback(void* data, int argc, char** argv, char** col_names) {
    SQLiteResult* result = (SQLiteResult*)data;

    // Primeira vez - alocar arrays e copiar nomes das colunas
    if (result->rows == 0 && argc > 0) {
        result->cols = argc;
        result->column_names = malloc(argc * sizeof(char*));
        if (!result->column_names) return 1; // Erro de alocação
        for (int i = 0; i < argc; i++) {
            size_t len = strlen(col_names[i]) + 1;
            result->column_names[i] = malloc(len);
            if (result->column_names[i]) {
#ifdef _MSC_VER
                strcpy_s(result->column_names[i], len, col_names[i]);
#else
                strncpy(result->column_names[i], col_names[i], len - 1);
                result->column_names[i][len - 1] = '\0';
#endif
            }
            else {
                // Tratar erro de alocação para nome de coluna
                for (int j = 0; j < i; ++j) free(result->column_names[j]);
                free(result->column_names);
                result->column_names = NULL;
                return 1;
            }
        }
    }

    // Realocar array de dados para a nova linha
    // Usar ponteiro temporário para evitar vazamento em caso de falha de realloc
    char** temp_data = realloc(result->data, (result->rows + 1) * result->cols * sizeof(char*));
    if (!temp_data) {
        // Erro de realocação, dados anteriores ainda em result->data
        // Idealmente, liberar memória já alocada para a linha atual se houver
        return 1; // Sinaliza erro
    }
    result->data = temp_data;

    // Copiar dados da linha atual
    for (int i = 0; i < argc; i++) {
        int index = result->rows * result->cols + i;
        if (argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            result->data[index] = malloc(len);
            if (result->data[index]) {
#ifdef _MSC_VER
                strcpy_s(result->data[index], len, argv[i]);
#else
                strncpy(result->data[index], argv[i], len - 1);
                result->data[index][len - 1] = '\0';
#endif
            }
            else {
                // Tratar erro de alocação para dado individual
                // Liberar memória alocada para esta linha até agora
                for (int k = 0; k < i; ++k) free(result->data[result->rows * result->cols + k]);
                // Reduzir o tamanho do realloc? Ou apenas retornar erro?
                return 1;
            }
        }
        else {
            // Alocar espaço para string vazia para representar NULL
            result->data[index] = malloc(1);
            if (result->data[index])
                result->data[index][0] = '\0';
            else return 1; // Erro de alocação
        }
    }

    result->rows++;
    return 0;
}

// Executa query SELECT
SQLiteResult* sqlite_select_query(SQLiteConnection* conn, const char* query) {
    if (!conn || !conn->db) return NULL;

    // Limpar erro anterior
    if (conn->last_error) {
        free(conn->last_error);
        conn->last_error = NULL;
    }

    SQLiteResult* result = malloc(sizeof(SQLiteResult));
    if (!result) return NULL; // Erro de alocação
    result->rows = 0;
    result->cols = 0;
    result->data = NULL;
    result->column_names = NULL;

    char* error_msg = NULL;
    int rc = sqlite3_exec(conn->db, query, select_callback, result, &error_msg);

    if (rc != SQLITE_OK) {
        if (error_msg) {
            size_t len = strlen(error_msg) + 1;
            conn->last_error = malloc(len);
            if (conn->last_error) {
#ifdef _MSC_VER
                strcpy_s(conn->last_error, len, error_msg);
#else
                strncpy(conn->last_error, error_msg, len - 1);
                conn->last_error[len - 1] = '\0';
#endif
            }
            sqlite3_free(error_msg);
        }
        sqlite_free_result(result); // Libera o que foi alocado antes do erro
        return NULL;
    }
    // Se sqlite3_exec retornou OK mas o callback falhou (e.g., alocação), o resultado pode estar incompleto
    // A função callback retorna não-zero em caso de erro, o que aborta sqlite3_exec.
    // Se rc == SQLITE_OK, o callback sempre retornou 0.

    // Se a query não retornou linhas, result->rows será 0, mas result não será NULL.
    // Se não retornou colunas (ex: SELECT 1 WHERE 0), result->cols será 0.

    return result;
}

// Libera resultado de consulta
void sqlite_free_result(SQLiteResult* result) {
    if (!result) return;

    // Liberar dados
    if (result->data) {
        // Itera sobre todos os ponteiros de dados alocados
        for (int i = 0; i < result->rows * result->cols; i++) {
            free(result->data[i]);
        }
        free(result->data);
    }

    // Liberar nomes das colunas
    if (result->column_names) {
        for (int i = 0; i < result->cols; i++) {
            free(result->column_names[i]);
        }
        free(result->column_names);
    }

    free(result);
}

// Retorna último erro
char* sqlite_get_last_error(SQLiteConnection* conn) {
    if (!conn) return NULL; // Se conn é NULL, não há erro para copiar
    if (!conn->last_error) return NULL; // Se não há erro registrado, retorna NULL

    size_t len = strlen(conn->last_error) + 1;
    char* error_copy = malloc(len);
    if (error_copy) {
#ifdef _MSC_VER
        strcpy_s(error_copy, len, conn->last_error);
#else
        strncpy(error_copy, conn->last_error, len - 1);
        error_copy[len - 1] = '\0';
#endif
    }
    return error_copy; // Retorna cópia do erro ou NULL se malloc falhar
}

// *** NOVA FUNÇÃO IMPLEMENTADA ***
// Retorna o número de linhas afetadas pela última query (INSERT, UPDATE, DELETE)
int sqlite_get_changes(SQLiteConnection* conn) {
    if (!conn || !conn->db) {
        // Opcional: Definir um erro ou retornar um valor específico como -1
        // Definindo erro para consistência com outras funções
        if (conn && conn->last_error) {
            free(conn->last_error);
            conn->last_error = NULL;
        }
        if (conn) {
            const char* msg = "Conexão SQLite inválida ou não inicializada para obter changes.";
            size_t len = strlen(msg) + 1;
            conn->last_error = malloc(len);
            if (conn->last_error) {
#ifdef _MSC_VER
                strcpy_s(conn->last_error, len, msg);
#else
                strncpy(conn->last_error, msg, len - 1);
                conn->last_error[len - 1] = '\0';
#endif
            }
        }
        return -1; // Retorna -1 em caso de erro de conexão
    }
    // Chama a função da API C do SQLite
    return sqlite3_changes(conn->db);
}

