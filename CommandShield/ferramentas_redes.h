// ferramentas_redes.h
#ifndef FERRAMENTAS_REDES_H
#define FERRAMENTAS_REDES_H

int handle_network_scan(const char* command);

void scan_ports(const char* host, int start_port, int end_port);

int ping_host(const char* host);

#endif
