#include "sqlite_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <stddef.h>
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

    // Tenta abrir o arquivo do banco
    int rc = sqlite3_open(db_path, &conn->db);
    if (rc != SQLITE_OK) {
        // Se deu erro, salva a mensagem de erro
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
    // Executa comandos que não retornam dados
    int rc = sqlite3_exec(conn->db, query, NULL, NULL, &error_msg);

    if (rc != SQLITE_OK) {
        // Guarda o erro se deu problema
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

// Consultas SELECT
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
    char** temp_data = realloc(result->data, (result->rows + 1) * result->cols * sizeof(char*));
    if (!temp_data) {
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
                return 1;
            }
        }
        else {
            // Alocar espaço para string vazia para representar NULL
            result->data[index] = malloc(1);
            if (result->data[index])
                result->data[index][0] = '\0';
            else return 1; 
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
    if (!conn) return NULL; 
    if (!conn->last_error) return NULL; 

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
    return error_copy; 
}

// Retorna o número de linhas afetadas pela última query (INSERT, UPDATE, DELETE)
int sqlite_get_changes(SQLiteConnection* conn) {
    if (!conn || !conn->db) {
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
        return -1; 
    }
    // Chama a função da API C do SQLite
    return sqlite3_changes(conn->db);
}

