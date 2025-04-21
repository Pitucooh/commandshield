// ferramentas_redes.h
#ifndef FERRAMENTAS_REDES_H
#define FERRAMENTAS_REDES_H

// Processa comandos de rede
int handle_network_scan(const char* command);

// Escaneia portas em um host
void scan_ports(const char* host, int start_port, int end_port);

// Verifica disponibilidade de um host
int ping_host(const char* host);

#endif
