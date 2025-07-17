#include <iostream>              // для виводу на екран
#include <winsock2.h>           // головна бібліотека сокетів для Windows
#include <ws2tcpip.h>           // додаткові функції типу inet_pton()
#include <string>               // будемо юзати std::string

#pragma comment(lib, "ws2_32.lib")  // щоб не забути лінкувати бібліотеку сокетів

#define PORT 8080               // порт на якому слухаємо клієнтів
#define BUFFER_SIZE 1024        // розмір буфера, ну тіпа 1024 символа — вистачить з головою

int main() {
    WSADATA wsaData;           // структура для ініціалізації WinSock
    SOCKET serverSocket;       // це сокет який буде слухати клієнтів
    SOCKET clientSocket;       // сокет який обслуговує вже конкретного клієнта
    sockaddr_in serverAddr;    // структура з інфою про сервер (IP, порт і тд)
    sockaddr_in clientAddr;    // для збереження даних клієнта
    int clientAddrSize = sizeof(clientAddr); // розмір структури клієнта
    char buffer[BUFFER_SIZE];  // буфер для обміну повідомленнями

    // Стартуємо сокети — must have для Windows
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Не вдалося запустити WSA\n";
        return 1;
    }

    // Створюємо TCP-сокет
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Помилка створення сокету\n";
        WSACleanup();
        return 1;
    }

    // Налаштування адреси сервера (localhost + порт)
    serverAddr.sin_family = AF_INET;               // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;       // приймаємо підключення з будь-якої адреси
    serverAddr.sin_port = htons(PORT);             // порт треба в форматі "мережевому"

    // Прив'язуємо сокет до адреси
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind не вдався\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Починаємо слухати (макс. кількість підключень по дефолту — багато)
    listen(serverSocket, SOMAXCONN);
    std::cout << "Сервер слухає...\n";

    // Приймаємо з'єднання (блокує, поки хтось не підключиться)
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "accept помилка\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Клієнт підключився!\n";

    // Цикл обробки повідомлень від клієнта
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);  // очищаємо буфер перед кожним прийомом
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            std::cout << "Клієнт розірвав з'єднання\n";
            break;
        }

        int number = atoi(buffer);          // перетворюємо строку в число
        number += 1;                        // типу "логіка сервера" — додаємо 1
        std::string response = std::to_string(number);  // назад у строку

        send(clientSocket, response.c_str(), response.length(), 0);  // надсилаємо назад клієнту
    }

    // Закриваємо сокети і очищаємо WinSock
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
