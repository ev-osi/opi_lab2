#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/statvfs.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", PORT);

    while (1) {
        // Получение пути от клиента
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("Received path: %s\n", buffer);

        // Проверка свободного и занятого места
        struct statvfs stat;
        if (statvfs(buffer, &stat) != 0) {
            perror("statvfs failed");
            snprintf(buffer, BUFFER_SIZE, "Error");
            sendto(server_fd, buffer, strlen(buffer), 0, (const struct sockaddr *)&client_addr, addr_len);
            continue;
        }

        unsigned long long free_space = stat.f_bfree * stat.f_bsize;
        unsigned long long total_space = stat.f_blocks * stat.f_bsize;
        unsigned long long used_space = total_space - free_space;

        // Отправка информации клиенту
        snprintf(buffer, BUFFER_SIZE, "%llu %llu", free_space, used_space);
        sendto(server_fd, buffer, strlen(buffer), 0, (const struct sockaddr *)&client_addr, addr_len);
        printf("Sent data: Free space: %llu, Used space: %llu\n", free_space, used_space);
    }

    close(server_fd);
    return 0;
}
