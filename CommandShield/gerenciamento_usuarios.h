#ifndef GERENCIAMENTO_USUARIOS_H
#define GERENCIAMENTO_USUARIOS_H

#include <mysql/mysql.h>

typedef struct {
    char username[100];
    int is_admin;
} User;

extern User current_user;

int authenticate_user();
int is_admin_user();

#endif
