#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char ip_address[16];

    // Solicitar ao usuário para inserir o endereço IP do servidor
    std::cout << "Enter server IP address: ";
    std::cin >> ip_address;

    const char *hello = "Hello from client";
    char buffer[1024] = {0};

    // Criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "\nSocket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converta o endereço IP inserido pelo usuário
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        std::cerr << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    // Conexão ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\nConnection Failed \n";
        return -1;
    }

    // Enviar mensagem ao servidor
    send(sock, hello, strlen(hello), 0);
    std::cout << "Hello message sent\n";

    // Receber resposta do servidor
    read(sock, buffer, 1024);
    std::cout << "Message received: " << buffer << std::endl;

    // Fechar socket
    close(sock);
    return 0;
}
