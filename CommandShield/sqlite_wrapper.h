#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H

#include "../sqlite3.h"

typedef struct {
    sqlite3* db;
    char* last_error;
} SQLiteConnection;

typedef struct {
    int rows;
    int cols;
    char** data;
    char** column_names;
} SQLiteResult;

// Funções principais
SQLiteConnection* sqlite_connect(const char* db_path);
void sqlite_disconnect(SQLiteConnection* conn);
int sqlite_execute_query(SQLiteConnection* conn, const char* query);
SQLiteResult* sqlite_select_query(SQLiteConnection* conn, const char* query);
void sqlite_free_result(SQLiteResult* result);
char* sqlite_get_last_error(SQLiteConnection* conn);

#endif