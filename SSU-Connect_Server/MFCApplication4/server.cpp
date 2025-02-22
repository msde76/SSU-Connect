#include "pch.h"
#include "server.h"
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 256

std::vector<SOCKET> clients; // Ŭ���̾�Ʈ ���� ����


// Ŭ���̾�Ʈ �޽����� ��ε�ĳ��Ʈ
void broadcastMessage(const char* message, SOCKET sender) {

    for (SOCKET client : clients) {
        if (client != sender) { // �ڱ� �ڽſ��Դ� �޽��� ���� �� ��
            send(client, message, strlen(message), 0);
        }
    }
}

// Ŭ���̾�Ʈ ó�� �Լ�
void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived <= 0) {
            std::cerr << "Ŭ���̾�Ʈ ���� ����.\n";
            closesocket(clientSocket);

            // Ŭ���̾�Ʈ ��Ͽ��� ����

            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }

        std::cout << "���� �޽���: " << buffer << std::endl;

        // ���� �޽����� �ٸ� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
        broadcastMessage(buffer, clientSocket);
    }
}

// ���� ���� �Լ�
int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr;

    // WinSock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup ����\n";
        return 1;
    }

    // ���� ���� ����
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "���� ���� ����\n";
        WSACleanup();
        return 1;
    }

    // ���� �ּ� ����
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // ���� ���ε�
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "���ε� ����\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // ������ ����
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "������ ����\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "������ ��Ʈ " << PORT << "���� ���۵Ǿ����ϴ�.\n";

    while (true) {
        SOCKADDR_IN clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Ŭ���̾�Ʈ ���� ����\n";
            continue;
        }

        // Ŭ���̾�Ʈ ��Ͽ� �߰�
        {

            clients.push_back(clientSocket);
        }

        // Ŭ���̾�Ʈ ó�� ������ ����
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    // ���� ���� �ݱ�
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
