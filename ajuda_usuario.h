// ajuda_usuario.h
#ifndef AJUDA_USUARIO_H
#define AJUDA_USUARIO_H

// Mostra o menu principal de ajuda
void show_help_menu();

// Mostra ajuda detalhada para um comando específico
void show_command_help(const char* command);

// Processa o comando help
int process_help_command(const char* command);

#endif