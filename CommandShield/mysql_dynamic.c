//#include <windows.h>
//#include <stdio.h>
//
//// Definir ponteiros para funções MySQL
//typedef void* (*mysql_init_func)(void*);
//typedef void* (*mysql_real_connect_func)(void*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long);
//typedef int (*mysql_query_func)(void*, const char*);
//typedef void (*mysql_close_func)(void*);
//
//static HMODULE mysql_dll = NULL;
//static mysql_init_func mysql_init_ptr = NULL;
//static mysql_real_connect_func mysql_real_connect_ptr = NULL;
//static mysql_query_func mysql_query_ptr = NULL;
//static mysql_close_func mysql_close_ptr = NULL;
//
//int load_mysql_dll() {
//    mysql_dll = LoadLibrary(L"libmysql.dll");
//    if (!mysql_dll) {
//        printf("Erro: Não foi possível carregar libmysql.dll\n");
//        return 0;
//    }
//
//    mysql_init_ptr = (mysql_init_func)GetProcAddress(mysql_dll, "mysql_init");
//    mysql_real_connect_ptr = (mysql_real_connect_func)GetProcAddress(mysql_dll, "mysql_real_connect");
//    mysql_query_ptr = (mysql_query_func)GetProcAddress(mysql_dll, "mysql_query");
//    mysql_close_ptr = (mysql_close_func)GetProcAddress(mysql_dll, "mysql_close");
//
//    return (mysql_init_ptr && mysql_real_connect_ptr && mysql_query_ptr && mysql_close_ptr);
//}