// Daniel Froés - 10255956
// Luana Balador Belisário - 10692245
// Pedro Henrique Nieuwenhoff - 10377729
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <list>
#include <thread>

#define PORT 8080
#define MAX_MSG_LENGTH 2048
#define USERNAME_LENGTH 25

// func readMessage: Lê uma mensagem que foi enviada para o cliente
// @param: socket do cliente
void ReadMessage(int sock) {
    char buffer[MAX_MSG_LENGTH + USERNAME_LENGTH];
    int msgLength;

    while (true) {
        msgLength = read(sock, buffer, MAX_MSG_LENGTH + USERNAME_LENGTH);
        buffer[msgLength] = '\0';
        printf("%s", buffer);
    }
}

// func readMessage: Lê uma mensagem que foi enviada para o cliente
// @param: socket do cliente
void SendMessage(std::string userName, int sock) {
    std::string msg;
    std::string msgAux;
    std::string msgDest;

    while (true) {
        std::getline(std::cin, msg);
        printf("mas envidada: %s\n", msg.c_str());

        // faz o tratamento do tamanho da msg recebida pelo client e enviada
        // por esse. Caso a msg tenha mais que 2048 caracteres, ela é dividida
        // em mais de uma automaticamente
        for (int offset = 0; offset < msg.length(); offset += MAX_MSG_LENGTH) {
            msgAux = msg.substr(offset, MAX_MSG_LENGTH);
            msgDest = userName + ": " + msgAux + "\n";
            send(sock, msgDest.c_str(), msgDest.length(), 0);
        }
    }
}

// func CreateSocket: Tenta criar um novo socket
// em caso de sucesso, retorna o socket criado
// caso contrário, para o programa com um erro
int CreateSocket() {
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    return sock;
}

// func ConnectToServer: conecta o novo client ao IP do
// server que está atualmente rodando na rede
// @param: socket do client
void ConnectToServer(int clientSock) {
    int valread;
    char welcomeMsg[1000] = {0};
    struct sockaddr_in serv_addr;

    // configurando o endereço do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <=
        0)  // Convert IPv4 and IPv6 addresses from text to binary form
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }

    // procura o servidor e conecta a socket
    if (connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0) {
        printf("\nConnection Failed \n");
        exit(-1);
    }

    valread = read(clientSock, welcomeMsg, 1000);
    printf("%s\n", welcomeMsg);
}

int main(int argc, char const *argv[]) {
    std::string userName;

    // criando a socket do cliente
    int clientSock = CreateSocket();
    ConnectToServer(clientSock);

    // design, msg de boas vindas do chat
    // escolher nome de usuário etc
    printf("Digite um nome de usuário para entrar no chat: ");
    std::cin >> userName;
    std::cout
        << "\n\n Bem vindo ao chat, " << userName
        << "\n Para enviar sua mensagem basta digitar e apertar Enter\n\n";

    // definição execução das threads de envio e recebimento
    // de mensagens entre os clients e o server
    std::thread readThread(ReadMessage, clientSock);

    std::thread sendThread(SendMessage, userName, clientSock);

    readThread.join();
    sendThread.join();

    return 0;
}