#include <iostream>              // щоб юзати cout і cin
#include <winsock2.h>           // сокети Windows
#include <ws2tcpip.h>           // inet_pton
#include <string>               // string для введення з клави

#pragma comment(lib, "ws2_32.lib")  // підключення бібліотеки

#define SERVER_IP "127.0.0.1"   // локальний сервер (localhost)
#define PORT 8080               // той самий порт що і у сервера
#define BUFFER_SIZE 1024        // розмір буфера

int main() {
    WSADATA wsaData;            // для старту сокетів
    SOCKET clientSocket;        // сам клієнтський сокет
    sockaddr_in serverAddr;     // інфа про сервер (IP, порт)
    char buffer[BUFFER_SIZE];   // буфер для прийому

    // Ініціалізація сокетів Windows
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Не вдалося запустити WSA\n";
        return 1;
    }

    // Створення сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не вдалося створити сокет\n";
        WSACleanup();
        return 1;
    }

    // Налаштування IP та порта сервера
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);  // конвертуємо строку в IP
    serverAddr.sin_port = htons(PORT);                    // порт

    // Підключення до сервера
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Підключення не вдалося\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Підключено до сервера!\n";

    // Нескінченний цикл запитів
    while (true) {
        std::string input;
        std::cout << "Введи число (або exit): ";
        std::cin >> input;

        if (input == "exit") break;  // вихід по команді

        send(clientSocket, input.c_str(), input.length(), 0);  // надсилаємо введене число

        memset(buffer, 0, BUFFER_SIZE);  // очищаємо буфер
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            std::cout << "З'єднання завершено сервером.\n";
            break;
        }

        std::cout << "Сервер каже: " << buffer << "\n";  // друкуємо відповідь
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
