#include "mysql_wrapper.h"
#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

using namespace mysqlx;

// Estrutura interna (oculta do C)
struct MySQLConnection {
    Session* session;
    std::string last_error;

    MySQLConnection() : session(nullptr) {}
    ~MySQLConnection() {
        if (session) {
            delete session;
        }
    }
};

extern "C" {

    MySQLConnection* mysql_connect(const char* host, int port, const char* user, const char* password, const char* database) {
        MySQLConnection* conn = new MySQLConnection();

        try {
            // Criar string de conexão
            std::string connection_string = std::string(user) + ":" + password + "@" + host + ":" + std::to_string(port) + "/" + database;

            conn->session = new Session(connection_string);
            conn->last_error = "";
            return conn;
        }
        catch (const Error& err) {
            conn->last_error = err.what();
            return conn; // Retorna mesmo com erro, para poder pegar a mensagem
        }
    }

    int mysql_execute_query(MySQLConnection* conn, const char* query) {
        if (!conn || !conn->session) {
            return -1;
        }

        try {
            conn->session->sql(query).execute();
            conn->last_error = "";
            return 0; // Sucesso
        }
        catch (const Error& err) {
            conn->last_error = err.what();
            return -1; // Erro
        }
    }

    QueryResult* mysql_select_query(MySQLConnection* conn, const char* query) {
        if (!conn || !conn->session) {
            return nullptr;
        }

        try {
            SqlResult result = conn->session->sql(query).execute();

            QueryResult* qr = new QueryResult();
            std::vector<std::vector<std::string>> rows_data;

            // Processar resultados
            for (Row row : result) {
                std::vector<std::string> row_data;
                for (unsigned int i = 0; i < row.colCount(); ++i) {
                    if (row[i].isNull()) {
                        row_data.push_back("NULL");
                    }
                    else {
                        row_data.push_back(std::string(row[i]));
                    }
                }
                rows_data.push_back(row_data);
            }

            // Converter para formato C
            qr->rows = rows_data.size();
            qr->cols = rows_data.empty() ? 0 : rows_data[0].size();

            if (qr->rows > 0) {
                qr->data = (char**)malloc(qr->rows * qr->cols * sizeof(char*));

                for (int i = 0; i < qr->rows; ++i) {
                    for (int j = 0; j < qr->cols; ++j) {
                        int index = i * qr->cols + j;
                        qr->data[index] = (char*)malloc(rows_data[i][j].length() + 1);
                        strcpy_s(qr->data[index], rows_data[i][j].length() + 1, rows_data[i][j].c_str());
                    }
                }
            }
            else {
                qr->data = nullptr;
            }

            conn->last_error = "";
            return qr;

        }
        catch (const Error& err) {
            conn->last_error = err.what();
            return nullptr;
        }
    }

    void mysql_free_result(QueryResult* result) {
        if (result) {
            if (result->data) {
                for (int i = 0; i < result->rows * result->cols; ++i) {
                    free(result->data[i]);
                }
                free(result->data);
            }
            delete result;
        }
    }

    char* mysql_get_last_error(MySQLConnection* conn) {
        if (!conn) {
            return nullptr;
        }

        // Alocar memória para retornar a string
        char* error_msg = (char*)malloc(conn->last_error.length() + 1);
        strcpy_s(error_msg, conn->last_error.length() + 1, conn->last_error.c_str());
        return error_msg;
    }

    void mysql_disconnect(MySQLConnection* conn) {
        if (conn) {
            delete conn;
        }
    }

} // extern "C"