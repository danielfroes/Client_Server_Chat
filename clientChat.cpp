// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <thread>
#include <list>

#define PORT 8080
#define MAX_MSG_LENGTH 4096
#define USERNAME_LENGTH 100

void readMessage(int sock)
{
    char buffer[MAX_MSG_LENGTH + USERNAME_LENGTH];
    int valread;

    while (true)
    {
        valread = read(sock, buffer, MAX_MSG_LENGTH + USERNAME_LENGTH);
        buffer[valread] = '\0';
        printf("%s", buffer);
    }

}

void sendMessage(std::string userName, int sock)
{
    std::string msg;
    std::string msgAux;
    std::string msgDest;

    while (true)
    {

        std::cin >> msg;

        for (int offset = 0; offset < msg.length(); offset += MAX_MSG_LENGTH)
        {
            msgAux = msg.substr(offset, MAX_MSG_LENGTH);
            msgDest = userName + ": " + msgAux + "\n";
            send(sock, msgDest.c_str(), msgDest.length(), 0);
    
        }
       
    }
}

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char welcomeMsg[1000] = {0};
    std::string userName;

    //criando a socket do cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    //configurando o endereço do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) // Convert IPv4 and IPv6 addresses from text to binary form
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    //procura o servidor e conecta a socket
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    valread = read(sock, welcomeMsg, 1000);
    printf("%s\n", welcomeMsg);

    printf("Digite um nome de usuário para entrar no chat: ");
    std::cin >> userName;
    std::cout << "\n\n Bem vindo ao chat, " << userName << "\n Para enviar sua mensagem basta digitar e apertar Enter\n\n";

    std::thread readThread(readMessage, sock);

    std::thread sendThread(sendMessage, userName, sock);

    readThread.join();
    sendThread.join();

    return 0;
}