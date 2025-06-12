// processador_comandos.h
#ifndef PROCESSADOR_COMANDOS_H
#define PROCESSADOR_COMANDOS_H

void display_welcome_banner();

void init_system();

void cleanup_system();

int process_command(const char* command);

#endif