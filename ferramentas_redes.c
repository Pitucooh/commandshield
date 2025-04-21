// ferramentas_redes.c
#include "ferramentas_redes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>  // Para inet_pton

// Precisamos linkar com ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

int handle_network_scan(const char* command) {
    char host[100] = { 0 };
    int start_port = 0, end_port = 0;
    char cmd[20] = { 0 };

    // Parse comando com validação adequada
    if (sscanf_s(command, "%19s %99s %d %d",
        cmd, (unsigned int)sizeof(cmd),
        host, (unsigned int)sizeof(host),
        &start_port, &end_port) == 4) {
        if (strcmp(cmd, "scan") == 0) {
            scan_ports(host, start_port, end_port);
            return 1;
        }
    }

    // Comando ping
    if (sscanf_s(command, "%19s %99s",
        cmd, (unsigned int)sizeof(cmd),
        host, (unsigned int)sizeof(host)) == 2) {
        if (strcmp(cmd, "ping") == 0) {
            if (ping_host(host)) {
                printf("Host %s está acessível.\n", host);
            }
            else {
                printf("Host %s não está acessível.\n", host);
            }
            return 1;
        }
    }

    printf("Uso incorreto do comando. Exemplos:\n");
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
        printf("Endereço IP inválido ou não pôde ser convertido: %s\n", host);
        WSACleanup();
        return;
    }

    printf("Endereço IP convertido com sucesso. Iniciando scan...\n");

    for (int port = start_port; port <= end_port; port++) {
        // Relatar progresso a cada 10 portas
        if (port % 10 == 0) {
            printf("Verificando porta %d...\n", port);
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
    int result;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao inicializar o Winsock. Erro: %d\n", WSAGetLastError());
        return 0;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Falha ao criar socket. Erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    // Configurar servidor
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;

    // Converter o endereço IP
    if (inet_pton(AF_INET, host, &(server.sin_addr)) != 1) {
        // Tentar resolver nome de domínio
        struct addrinfo hints, * res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(host, NULL, &hints, &res) != 0) {
            printf("Não foi possível resolver o nome de host: %s\n", host);
            closesocket(sock);
            WSACleanup();
            return 0;
        }

        server.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
        freeaddrinfo(res);
    }

    server.sin_port = htons(80); // Tenta conectar na porta 80 (HTTP)

    // Definir timeout
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    // Tentar conectar
    result = connect(sock, (struct sockaddr*)&server, sizeof(server));

    closesocket(sock);
    WSACleanup();

    return (result != SOCKET_ERROR);
}