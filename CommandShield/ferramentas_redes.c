// ferramentas_redes.c
#include "ferramentas_redes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>  // Para inet_pton e getaddrinfo

// Precisamos linkar com ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

int handle_network_scan(const char* command) {
    // Comando ping
    if (strncmp(command, "ping", 4) == 0) {
        char host[100] = { 0 };

        // Extrair o host após "ping "
        if (sscanf_s(command, "ping %99s", host, (unsigned int)sizeof(host)) == 1) {
            printf("Executando ping para %s...\n", host);

            if (ping_host(host)) {
                printf("✓ Host %s está acessível.\n", host);
            }
            else {
                printf("✗ Host %s não está acessível.\n", host);
            }
            return 1;
        }
        else {
            printf("Uso incorreto. Exemplo: ping 192.168.1.1 ou ping google.com\n");
            return 1;
        }
    }

    // Comandos de scan
    if (strncmp(command, "scan", 4) == 0) {
        char host[100] = { 0 };
        int start_port = 0, end_port = 0;

        // Parse comando com validação adequada
        if (sscanf_s(command, "scan %99s %d %d",
            host, (unsigned int)sizeof(host),
            &start_port, &end_port) == 3) {
            scan_ports(host, start_port, end_port);
            return 1;
        }
        else {
            printf("Uso incorreto. Formato: scan [host] [porta_inicial] [porta_final]\n");
            printf("Exemplo: scan 192.168.1.1 1 1000\n");
            return 1;
        }
    }

    // Se chegou aqui, o comando não foi reconhecido como scan ou ping
    printf("Comando de rede não reconhecido. Use 'scan' ou 'ping'.\n");
    printf("Exemplos:\n");
    printf("  scan 192.168.1.1 1 1000 - Escaneia portas de 1 a 1000\n");
    printf("  ping 192.168.1.1 - Verifica se o host está acessível\n");
    return 1;
}

void scan_ports(const char* host, int start_port, int end_port) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    int result;
    int portsFound = 0;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao inicializar o Winsock. Erro: %d\n", WSAGetLastError());
        return;
    }

    printf("Escaneando portas %d a %d em %s...\n", start_port, end_port, host);

    // Configurar servidor
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    // Converter o endereço IP
    if (inet_pton(AF_INET, host, &(server.sin_addr)) != 1) {
        printf("Tentando resolver nome de domínio: %s\n", host);

        // Tentar resolver nome de domínio
        struct addrinfo hints, * res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host, NULL, &hints, &res) != 0) {
            printf("Não foi possível resolver o nome de host: %s (Erro: %d)\n",
                host, WSAGetLastError());
            WSACleanup();
            return;
        }

        server.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(server.sin_addr), ip_str, INET_ADDRSTRLEN);
        printf("Endereço resolvido: %s -> %s\n", host, ip_str);

        freeaddrinfo(res);
    }

    printf("Iniciando scan de portas...\n");

    for (int port = start_port; port <= end_port; port++) {
        // Relatar progresso a cada 100 portas
        if ((port - start_port) % 100 == 0 && port > start_port) {
            printf("Progresso: verificadas %d de %d portas...\n",
                port - start_port, end_port - start_port + 1);
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            printf("Erro ao criar socket para porta %d. Erro: %d\n", port, WSAGetLastError());
            continue;
        }

        server.sin_port = htons(port);

        // Definir timeout para a conexão (curto para aumentar a velocidade do scan)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

        // Tentar conectar
        result = connect(sock, (struct sockaddr*)&server, sizeof(server));

        if (result != SOCKET_ERROR) {
            printf("Porta %d: ABERTA\n", port);
            portsFound++;
        }

        closesocket(sock);
    }

    // Relatório final
    if (portsFound == 0) {
        printf("Scan concluído. Nenhuma porta aberta encontrada no intervalo %d-%d.\n", start_port, end_port);
    }
    else {
        printf("Scan concluído. Total de portas abertas encontradas: %d\n", portsFound);
    }

    WSACleanup();
}

int ping_host(const char* host) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    int result = 0;
    int ports_to_try[] = { 80, 443, 22, 21, 25, 3389 }; // Portas comuns
    int num_ports = sizeof(ports_to_try) / sizeof(ports_to_try[0]);

    printf("Tentando fazer ping para o host: %s\n", host);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao inicializar o Winsock. Erro: %d\n", WSAGetLastError());
        return 0;
    }

    // Configurar servidor
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    // Tentar converter o endereço IP
    if (inet_pton(AF_INET, host, &(server.sin_addr)) != 1) {
        printf("Tentando resolver nome de domínio: %s\n", host);

        // Tentar resolver nome de domínio
        struct addrinfo hints, * res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host, NULL, &hints, &res) != 0) {
            printf("Não foi possível resolver o nome de host: %s\n", host);
            WSACleanup();
            return 0;
        }

        server.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(server.sin_addr), ip_str, INET_ADDRSTRLEN);
        printf("Endereço resolvido: %s -> %s\n", host, ip_str);

        freeaddrinfo(res);
    }

    printf("Tentando várias portas para verificar conectividade...\n");

    // Tentar várias portas comuns
    for (int i = 0; i < num_ports; i++) {
        int port = ports_to_try[i];
        printf("Tentando porta %d...\n", port);

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            continue;
        }

        // Definir timeout
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

        server.sin_port = htons(port);
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
            printf("Conexão estabelecida na porta %d.\n", port);
            result = 1;
            closesocket(sock);
            break;
        }

        closesocket(sock);
    }

    // Se nenhuma porta respondeu, vamos tentar um teste mais simples
    if (!result) {
        printf("Nenhuma porta respondeu. Verificando se o IP é alcançável...\n");

        // Usar função ping do sistema
        char cmd[256];
        sprintf_s(cmd, sizeof(cmd), "ping -n 1 -w 1000 %s > NUL", host);
        result = (system(cmd) == 0);

        if (result) {
            printf("Host responde ao ping ICMP.\n");
        }
        else {
            printf("Host não responde ao ping ICMP.\n");
        }
    }

    WSACleanup();
    return result;
}