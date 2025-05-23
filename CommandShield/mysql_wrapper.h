#ifndef MYSQL_WRAPPER_H
#define MYSQL_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

    // Estrutura opaca para esconder detalhes do C++
    typedef struct MySQLConnection MySQLConnection;

    // Funções para usar no código C
    MySQLConnection* mysql_connect(const char* host, int port, const char* user, const char* password, const char* database);
    int mysql_execute_query(MySQLConnection* conn, const char* query);
    char* mysql_get_last_error(MySQLConnection* conn);
    void mysql_disconnect(MySQLConnection* conn);

    // Funções para consultas SELECT
    typedef struct {
        char** data;
        int rows;
        int cols;
    } QueryResult;

    QueryResult* mysql_select_query(MySQLConnection* conn, const char* query);
    void mysql_free_result(QueryResult* result);

#ifdef __cplusplus
}
#endif

#endif // MYSQL_WRAPPER_H