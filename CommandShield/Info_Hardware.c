#include "Info_Hardware.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <psapi.h>

// Função para listar os drivers de dispositivo
void list_device_drivers() {
    FILE* fp;
    char line[512];
    int count = 0;

    printf("\n--- Listando Drivers de Dispositivo Carregados ---\n");

    // Criar um processo para executar o comando driverquery
    fp = _popen("driverquery /FO CSV /NH", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando driverquery.\n");
        printf("Certifique-se de estar executando como administrador.\n");
        return;
    }

    // Ler a primeira linha (cabeçalho) e descartá-la
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("Erro ao ler a saída do comando driverquery.\n");
        _pclose(fp);
        return;
    }

    // Ler e processar cada linha da saída
    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;

        // Remover aspas e separar campos
        char* nome = strtok(line, ",");
        if (nome != NULL) {
            // Remover aspas
            if (nome[0] == '"') {
                nome++;
                nome[strlen(nome) - 1] = '\0';
            }

            char* tipo = strtok(NULL, ",");
            char* estado = strtok(NULL, ",");

            if (tipo != NULL && estado != NULL) {
                // Remover aspas
                if (tipo[0] == '"') {
                    tipo++;
                    tipo[strlen(tipo) - 1] = '\0';
                }
                if (estado[0] == '"') {
                    estado++;
                    estado[strlen(estado) - 1] = '\0';
                }

                printf("%3d: %s (%s, %s)\n", count, nome, tipo, estado);
            }
            else {
                printf("%3d: %s\n", count, nome);
            }
        }
    }

    printf("\nTotal de drivers encontrados: %d\n", count);
    printf("--------------------------------------------------\n");

    _pclose(fp);
}

// Função para mostrar informações de memória
void show_memory_info() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    printf("\n--- Informações de Memória do Sistema ---\n");

    if (GlobalMemoryStatusEx(&memInfo)) {
        // Converter bytes para MB para melhor legibilidade
        double totalPhysMB = (double)memInfo.ullTotalPhys / (1024 * 1024);
        double availPhysMB = (double)memInfo.ullAvailPhys / (1024 * 1024);
        double usedPhysMB = totalPhysMB - availPhysMB;
        double totalVirtualMB = (double)memInfo.ullTotalVirtual / (1024 * 1024);
        double availVirtualMB = (double)memInfo.ullAvailVirtual / (1024 * 1024);
        double usedVirtualMB = totalVirtualMB - availVirtualMB;

        printf("Uso de memória: %lu%%\n", memInfo.dwMemoryLoad);
        printf("\nMemória Física (RAM):\n");
        printf("  Total:      %.2f MB (%.2f GB)\n", totalPhysMB, totalPhysMB / 1024);
        printf("  Disponível: %.2f MB (%.2f GB)\n", availPhysMB, availPhysMB / 1024);
        printf("  Em uso:     %.2f MB (%.2f GB)\n", usedPhysMB, usedPhysMB / 1024);

        printf("\nMemória Virtual:\n");
        printf("  Total:      %.2f MB (%.2f GB)\n", totalVirtualMB, totalVirtualMB / 1024);
        printf("  Disponível: %.2f MB (%.2f GB)\n", availVirtualMB, availVirtualMB / 1024);
        printf("  Em uso:     %.2f MB (%.2f GB)\n", usedVirtualMB, usedVirtualMB / 1024);

        // Informações sobre paginação
        PERFORMANCE_INFORMATION perfInfo;
        if (GetPerformanceInfo(&perfInfo, sizeof(perfInfo))) {
            SIZE_T pageSize = perfInfo.PageSize;
            SIZE_T commitTotal = perfInfo.CommitTotal * pageSize / (1024 * 1024);
            SIZE_T commitLimit = perfInfo.CommitLimit * pageSize / (1024 * 1024);

            printf("\nInformações de Paginação:\n");
            printf("  Tamanho da página:    %lu bytes\n", (unsigned long)pageSize);
            printf("  Total comprometido:   %lu MB\n", (unsigned long)commitTotal);
            printf("  Limite de commit:     %lu MB\n", (unsigned long)commitLimit);
        }
    }
    else {
        printf("Erro ao obter informações de memória. Código: %lu\n", GetLastError());
    }

    printf("--------------------------------------------------\n");
}

// Função para mostrar informações da CPU usando comandos do sistema
void show_cpu_info() {
    FILE* fp;
    char line[512];

    printf("\n--- Informações da CPU ---\n");

    // Usar wmic para obter informações da CPU
    fp = _popen("wmic cpu get Name, NumberOfCores, NumberOfLogicalProcessors, MaxClockSpeed /format:csv", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando wmic.\n");
        return;
    }

    // Pular a linha em branco
    fgets(line, sizeof(line), fp);

    // Ler a linha de cabeçalho
    fgets(line, sizeof(line), fp);

    // Ler os dados da CPU
    if (fgets(line, sizeof(line), fp) != NULL) {
        char* token = strtok(line, ",");  // Pular o nome do nó

        char* name = strtok(NULL, ",");
        char* cores = strtok(NULL, ",");
        char* logical = strtok(NULL, ",");
        char* speed = strtok(NULL, ",");

        if (name && cores && logical && speed) {
            // Remover caracteres de nova linha
            name[strcspn(name, "\r\n")] = 0;
            cores[strcspn(cores, "\r\n")] = 0;
            logical[strcspn(logical, "\r\n")] = 0;
            speed[strcspn(speed, "\r\n")] = 0;

            printf("Processador: %s\n", name);
            printf("Núcleos físicos: %s\n", cores);
            printf("Processadores lógicos: %s\n", logical);
            printf("Velocidade máxima: %.2f GHz\n", atof(speed) / 1000.0);
        }
    }

    _pclose(fp);

    // Obter informações de uso da CPU
    fp = _popen("wmic cpu get LoadPercentage /format:csv", "r");
    if (fp == NULL) {
        printf("Erro ao obter uso da CPU.\n");
    }
    else {
        // Pular linhas iniciais
        fgets(line, sizeof(line), fp);
        fgets(line, sizeof(line), fp);

        if (fgets(line, sizeof(line), fp) != NULL) {
            char* token = strtok(line, ",");  // Pular o nome do nó
            char* usage = strtok(NULL, ",");

            if (usage) {
                usage[strcspn(usage, "\r\n")] = 0;
                printf("Uso atual: %s%%\n", usage);
            }
        }

        _pclose(fp);
    }

    printf("--------------------------------------------------\n");
}

// Função para mostrar informações de disco usando comandos do sistema
void show_disk_info() {
    FILE* fp;
    char line[512];
    int count = 0;

    printf("\n--- Informações de Disco ---\n");

    // Usar wmic para obter informações de disco
    fp = _popen("wmic logicaldisk get Caption, FileSystem, Size, FreeSpace /format:csv", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando wmic.\n");
        return;
    }

    // Pular a linha em branco
    fgets(line, sizeof(line), fp);

    // Ler a linha de cabeçalho
    fgets(line, sizeof(line), fp);

    // Ler os dados dos discos
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(line) <= 1) continue;  // Pular linhas vazias

        char* token = strtok(line, ",");  // Pular o nome do nó

        char* caption = strtok(NULL, ",");
        char* filesystem = strtok(NULL, ",");
        char* freeSpace = strtok(NULL, ",");
        char* size = strtok(NULL, ",");

        if (caption && filesystem && freeSpace && size) {
            // Remover caracteres de nova linha
            caption[strcspn(caption, "\r\n")] = 0;
            filesystem[strcspn(filesystem, "\r\n")] = 0;
            freeSpace[strcspn(freeSpace, "\r\n")] = 0;
            size[strcspn(size, "\r\n")] = 0;

            // Converter para GB
            double sizeGB = atof(size) / (1024 * 1024 * 1024);
            double freeGB = atof(freeSpace) / (1024 * 1024 * 1024);
            double usedGB = sizeGB - freeGB;
            double usedPercent = (sizeGB > 0) ? (usedGB / sizeGB) * 100 : 0;

            printf("\nDisco %s:\n", caption);
            printf("  Sistema de arquivos: %s\n", filesystem);
            printf("  Tamanho total: %.2f GB\n", sizeGB);
            printf("  Espaço livre: %.2f GB\n", freeGB);
            printf("  Espaço usado: %.2f GB (%.1f%%)\n", usedGB, usedPercent);

            count++;
        }
    }

    _pclose(fp);

    if (count == 0) {
        printf("Nenhum disco encontrado.\n");
    }

    printf("--------------------------------------------------\n");
}

// Função para mostrar informações de rede usando comandos do sistema
void show_network_info() {
    FILE* fp;
    char line[1024];

    printf("\n--- Informações de Rede ---\n");

    // Usar ipconfig para obter informações de rede
    fp = _popen("ipconfig /all", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando ipconfig.\n");
        return;
    }

    printf("\nAdaptadores de rede e configurações IP:\n\n");

    // Ler e exibir a saída do ipconfig
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remover caracteres de nova linha
        line[strcspn(line, "\r\n")] = 0;
        printf("%s\n", line);
    }

    _pclose(fp);

    // Usar netstat para obter informações de conexões
    printf("\nConexões de rede ativas:\n\n");

    fp = _popen("netstat -an | findstr ESTABLISHED", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando netstat.\n");
        printf("--------------------------------------------------\n");
        return;
    }

    // Ler e exibir a saída do netstat
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remover caracteres de nova linha
        line[strcspn(line, "\r\n")] = 0;
        printf("%s\n", line);
    }

    _pclose(fp);

    printf("--------------------------------------------------\n");
}

// Função para mostrar informações de dispositivos USB usando comandos do sistema
void show_usb_info() {
    FILE* fp;
    char line[512];

    printf("\n--- Dispositivos USB Conectados ---\n\n");

    // Usar wmic para obter informações de dispositivos USB
    fp = _popen("wmic path Win32_USBControllerDevice get Dependent /format:list", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando wmic.\n");
        return;
    }

    int count = 0;

    // Ler e processar a saída
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remover caracteres de nova linha
        line[strcspn(line, "\r\n")] = 0;

        // Procurar por linhas com informações de dispositivos
        if (strstr(line, "Dependent=") != NULL) {
            count++;

            // Extrair o nome do dispositivo
            char* deviceInfo = strstr(line, "DeviceID=");
            if (deviceInfo) {
                printf("%d: %s\n", count, deviceInfo + 9);  // +9 para pular "DeviceID="
            }
            else {
                printf("%d: %s\n", count, line);
            }
        }
    }

    _pclose(fp);

    // Se não encontrou dispositivos pelo método anterior, tentar outro comando
    if (count == 0) {
        fp = _popen("wmic path Win32_PnPEntity where \"PNPClass='USB'\" get Caption,DeviceID /format:csv", "r");
        if (fp == NULL) {
            printf("Erro ao executar o comando wmic alternativo.\n");
            printf("--------------------------------------------------\n");
            return;
        }

        // Pular a linha em branco e o cabeçalho
        fgets(line, sizeof(line), fp);
        fgets(line, sizeof(line), fp);

        // Ler os dados dos dispositivos
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strlen(line) <= 1) continue;  // Pular linhas vazias

            // Remover caracteres de nova linha
            line[strcspn(line, "\r\n")] = 0;

            count++;
            printf("%d: %s\n", count, line);
        }

        _pclose(fp);
    }

    if (count == 0) {
        printf("Nenhum dispositivo USB encontrado ou erro ao obter informações.\n");
    }

    printf("--------------------------------------------------\n");
}

// Função para mostrar informações de processos usando comandos do sistema
void show_process_info() {
    FILE* fp;
    char line[512];

    printf("\n--- Processos em Execução ---\n\n");
    printf("%-6s %-30s %-10s\n", "PID", "Nome", "Memória (KB)");
    printf("--------------------------------------------------\n");

    // Usar tasklist para obter informações de processos
    fp = _popen("tasklist /FO CSV /NH", "r");
    if (fp == NULL) {
        printf("Erro ao executar o comando tasklist.\n");
        printf("--------------------------------------------------\n");
        return;
    }

    int count = 0;

    // Ler e processar a saída
    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;

        // Processar a linha CSV
        char* processName = strtok(line, ",");
        char* pid = strtok(NULL, ",");
        char* sessionName = strtok(NULL, ",");
        char* sessionNum = strtok(NULL, ",");
        char* memUsage = strtok(NULL, ",");

        if (processName && pid && memUsage) {
            // Remover aspas
            if (processName[0] == '"') {
                processName++;
                processName[strlen(processName) - 1] = '\0';
            }

            if (pid[0] == '"') {
                pid++;
                pid[strlen(pid) - 1] = '\0';
            }

            if (memUsage[0] == '"') {
                memUsage++;
                memUsage[strlen(memUsage) - 1] = '\0';
            }

            // Remover " KB" do final da string de memória
            char* kbPos = strstr(memUsage, " KB");
            if (kbPos) {
                *kbPos = '\0';
            }

            printf("%-6s %-30s %-10s\n", pid, processName, memUsage);
        }
    }

    _pclose(fp);

    printf("\nTotal de processos: %d\n", count);
    printf("--------------------------------------------------\n");
}

// Função para processar comandos de hardware
int handle_hardware_command(const char* command) {
    if (strcmp(command, "drivers") == 0) {
        list_device_drivers();
        return 1;
    }
    else if (strcmp(command, "meminfo") == 0) {
        show_memory_info();
        return 1;
    }
    else if (strcmp(command, "cpuinfo") == 0) {
        show_cpu_info();
        return 1;
    }
    else if (strcmp(command, "diskinfo") == 0) {
        show_disk_info();
        return 1;
    }
    else if (strcmp(command, "netinfo") == 0) {
        show_network_info();
        return 1;
    }
    else if (strcmp(command, "usbinfo") == 0) {
        show_usb_info();
        return 1;
    }
    else if (strcmp(command, "procinfo") == 0) {
        show_process_info();
        return 1;
    }
    else {
        printf("Subcomando de hardware não reconhecido: ");
        const char* space = strchr(command, ' ');
        if (space != NULL && *(space + 1) != '\0') {
            printf("'%s'\n", space + 1);
        }
        else {
            printf("'%s'\n", command);
        }
        printf("Comandos de hardware disponíveis:\n");
        printf("  drivers   - Lista os drivers de dispositivo\n");
        printf("  meminfo   - Mostra informações de memória do sistema\n");
        printf("  cpuinfo   - Mostra informações da CPU\n");
        printf("  diskinfo  - Mostra informações dos discos\n");
        printf("  netinfo   - Mostra informações de rede\n");
        printf("  usbinfo   - Mostra dispositivos USB conectados\n");
        printf("  procinfo  - Mostra processos em execução\n");
        return 1;
    }
}
