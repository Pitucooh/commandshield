// processador_comandos.h
#ifndef PROCESSADOR_COMANDOS_H
#define PROCESSADOR_COMANDOS_H

// Exibe o banner de boas-vindas com ASCII art
void display_welcome_banner();

// Inicializa o sistema
void init_system();

// Limpa recursos ao finalizar
void cleanup_system();

// Processa um comando e retorna o status (0 para sair, 1 para continuar)
int process_command(const char* command);

#endif