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


//func readMessage: Lê uma mensagem que foi enviada para o cliente
//@param: socket do cliente

void ReadMessage(int sock)
{
    char buffer[MAX_MSG_LENGTH + USERNAME_LENGTH];
    int msgLength;

    while (true)
    {
        msgLength = read(sock, buffer, MAX_MSG_LENGTH + USERNAME_LENGTH);
        buffer[msgLength] = '\0';
        printf("%s", buffer);
    }

}

//func readMessage: Lê uma mensagem que foi enviada para o cliente
//@param: socket do cliente
void SendMessage(std::string userName, int sock)
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


int CreateSocket()
{
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(-1);
    }
    
    return sock;
}

void ConnectToServer(int clientSock)
{
    int valread;
    char welcomeMsg[1000] = {0};
    struct sockaddr_in serv_addr;

    //configurando o endereço do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) // Convert IPv4 and IPv6 addresses from text to binary form
    {
        printf("\nInvalid address/ Address not supported \n");
        exit (-1);
    }

    //procura o servidor e conecta a socket
    if (connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit (-1);
    }

    valread = read(clientSock, welcomeMsg, 1000);
    printf("%s\n", welcomeMsg);
}



int main(int argc, char const *argv[])
{
    std::string userName;

    //criando a socket do cliente
    int clientSock = CreateSocket();
    ConnectToServer(clientSock);

    printf("Digite um nome de usuário para entrar no chat: ");
    std::cin >> userName;
    std::cout << "\n\n Bem vindo ao chat, " << userName << "\n Para enviar sua mensagem basta digitar e apertar Enter\n\n";

    std::thread readThread(ReadMessage, clientSock);

    std::thread sendThread(SendMessage, userName, clientSock);

    readThread.join();
    sendThread.join();

    return 0;
}

