#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "192.168.0.80"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int serverAddrSize = sizeof(serverAddr);

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Создание сокета
    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Ввод пути от пользователя
    printf("Enter the path to check disk space: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Удалить символ новой строки

    // Отправка пути серверу
    sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, serverAddrSize);

    // Получение ответа от сервера
    memset(buffer, 0, BUFFER_SIZE);
    recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &serverAddrSize);

    printf("Server response: %s\n", buffer);

    // Завершение работы
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
