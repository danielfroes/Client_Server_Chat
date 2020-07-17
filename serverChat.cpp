// Daniel Froés - 10255956
// Luana Balador Belisário - 10692245
// Pedro Henrique Nieuwenhoff - 10377729
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <map>

#define TRUE 1
#define FALSE 0
#define PORT 8080

#define MAX_USERS_NUMBER 100
#define MAX_MSG_LENGTH 2048
#define MAX_CHANNELNAME_LENGTH 200
#define MAX_CHANNELS_NUMBER 10


typedef struct _client
{
    int socket;
    char username[MAX_MSG_LENGTH];
    std::string currentChannel;
    bool isAdmin;
    bool isMuted;
} client;

typedef struct _channel
{   
    std::string channelName;
    client admin;
} channel;


int FindClient(client* clientArray, int maxClients, const char * nomeUsuario);
int InitServer(sockaddr_in *address, unsigned long *addrlen);
void DisconnectClient(client *clientArray, int pos, struct sockaddr_in address, int addrlen);
int SetSockets(client *clientArray, int maxClients, int *serverSocket, fd_set *readfds);
bool CheckForConnectionRequest(int *serverSocket, fd_set *readfds);
void ConnectWithClient(client *clientArray, int maxClients, int *serverSocket, struct sockaddr_in address, int addrlen);
bool CheckClientRequest(client cli, fd_set *readfds);
void CreateChannel(channel *channelArray, int maxChannel, std::string newChannelName, client* newChannelAdmin);


//** To do Servidor
// - Servidor detectar os cliente que não estão recebendo mensagem e apos 5 tentativas, desconectar o cliente

int main(int argc, char *argv[])
{

    int  maxClients = MAX_USERS_NUMBER, maxChannels = MAX_CHANNELS_NUMBER, activity, valread, lastSocket;
    int sendToClientErrorsCnt = 0;
    unsigned long addrlen;
    struct sockaddr_in address;
    // std::map<std::string, client> channels;
    
    
    channel channelArray[MAX_CHANNELS_NUMBER];
    client clientArray[MAX_USERS_NUMBER], cli;
    int serverSocket;

    char buffer[MAX_MSG_LENGTH], auxStr[MAX_MSG_LENGTH]; // data buffer of 1K
    
    // set of socket descriptors
    fd_set readfds;

    serverSocket = InitServer(&address, &addrlen);

    // initialise all clientArray[] to 0 so not checked
    for (int i = 0; i < maxClients; i++)
    {
        clientArray[i].socket = 0;
        clientArray[i].isAdmin = false;
    }
    for (int i = 0; i < maxChannels; i++)
    {
        channelArray[i].channelName = "";
    }


    puts("Waiting for connections ...");

    while (TRUE)
    {

        lastSocket = SetSockets(clientArray, maxClients, &serverSocket, &readfds);

        // wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(lastSocket + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        if (CheckForConnectionRequest(&serverSocket, &readfds))
        {
            ConnectWithClient(clientArray, maxClients, &serverSocket, address, addrlen);
        }

        // else its some IO operation on some other socket
        for (int i = 0; i < maxClients; i++)
        {
            cli = clientArray[i];
           
            if (CheckClientRequest(clientArray[i], &readfds))
            {
                // Check if it was for closing, and also read the
                // incoming message
                if ((valread = read(cli.socket, buffer, MAX_MSG_LENGTH)) == 0)
                {
                    DisconnectClient(clientArray, i, address, addrlen);
                }
                // Echo back the message that came in to other clients
                else
                {
                    //for some reason when the message is read, the string doesn't come with the terminator by default;
                    buffer[valread] = '\0';
                    std::string strBuffer = {buffer};

                    if(strBuffer.substr(0,6) == "/join ")
                    {
                        //So entra se o cliente não tiver entrando em nenhum canal
                        if(cli.currentChannel == "")
                        {
                            std::string channelName =  strBuffer.substr(6, strBuffer.length() - 6);
                        
                            bool channelExists = false;

                            //procurar se ja existia o canal e se sim conectar a ele    
                            for(int j = 0; j < maxChannels; j++)
                            {
                                if( channelArray[j].channelName == channelName)
                                {
                                    clientArray[i].currentChannel = channelName;
                                    std::cout << "Cliente " << cli.username << " entrou no canal " << channelName << " com sucesso" << std::endl;
                                    channelExists = true;
                                    break;
                                }
                            }
                            //se não existir criar um novo canal
                            if(!channelExists)
                            {
                                CreateChannel(channelArray, maxChannels, channelName, &clientArray[i]);
                                std::cout << "Canal " << channelName << " criado com sucesso" << std::endl;
                                clientArray[i].currentChannel = channelName;
                                std::cout << "Cliente " << cli.username << " entrou no canal " << channelName << " com sucesso" << std::endl;
                            }

                        }     
                    }
                    //when the clients send a message with "/ping" the server send a message "/pong" back to the client
                    else if (strcmp(buffer, "/ping\n") == 0)
                    {
                        strcpy(buffer, "pong\n");
                        send(cli.socket, buffer, strlen(buffer), 0);
                    }
                    else if(strBuffer.substr(0,6) == "/kick ")
                    {
                        if(cli.isAdmin == true){
                            std::string nomeUsuario = strBuffer.substr(6,strBuffer.length()-7);
                            int pos = FindClient(clientArray, maxClients, nomeUsuario.c_str());

                            // DisconnectClient(clientArray, pos, address, addrlen);

                            clientArray[pos].currentChannel = "";
                            std::string auxiliar = "/disconnect";
                            send(clientArray[pos].socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }
                        else{
                            std::string auxiliar = "Esse comando é exclusivo para o administrador do canal!\n";
                            send(cli.socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }

                    }
                    else if(strBuffer.substr(0,6) == "/mute ")
                    {
                        if(cli.isAdmin == true){
                            std::string nomeUsuario = strBuffer.substr(6,strBuffer.length()-7);
                            int pos = FindClient(clientArray, maxClients, nomeUsuario.c_str());

                            clientArray[pos].isMuted = true;
                        }
                        else{
                            std::string auxiliar = "Esse comando é exclusivo para o administrador do canal!\n";
                            send(cli.socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }
                    }
                    else if(strBuffer.substr(0,8) == "/unmute ")
                    {
                        if(cli.isAdmin == true){
                            std::string nomeUsuario = strBuffer.substr(8,strBuffer.length()-9);
                            int pos = FindClient(clientArray, maxClients, nomeUsuario.c_str());

                            clientArray[pos].isMuted = false;
                        }
                        else{
                            std::string auxiliar = "Esse comando é exclusivo para o administrador do canal!\n";
                            send(cli.socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }
                    }
                    else if(strBuffer.substr(0,7) == "/whois ")
                    {
                        if(cli.isAdmin == true){
                            std::string nomeUsuario = strBuffer.substr(7,strBuffer.length()-8);
                            int pos = FindClient(clientArray, maxClients, nomeUsuario.c_str());

                            getpeername(clientArray[pos].socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                            std::string auxiliar = "O IP do usuário " + nomeUsuario + " é " + inet_ntoa(address.sin_addr) +
                                                    ", porta " + std::to_string(ntohs(address.sin_port)) + ".\n";

                            send(cli.socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }
                        else{
                            std::string auxiliar = "Esse comando é exclusivo para o administrador do canal!\n";
                            send(cli.socket, auxiliar.c_str(), strlen(auxiliar.c_str()), 0);
                        }
                        
                    }
                    else
                    {

                        for (int j = 0; j < maxClients; j++)
                        {

                            //send the message to all clients, that way creating the chat
                            if (clientArray[j].socket != 0 and clientArray[j].currentChannel == cli.currentChannel and cli.isMuted == false and cli.currentChannel != "")
                            {
                            
                                std::cout<< std::endl << "cliente " << cli.username << " mandando mensagem para " << clientArray[j].username << " no canal " << cli.currentChannel <<std::endl;
                                if(cli.isAdmin)
                                {
                                    strcpy(auxStr, "(admin) ");
                                    strcat(auxStr, cli.username);
                                }
                                else
                                {
                                    strcpy(auxStr, cli.username);
                                }
                               
                                strcat(auxStr, ": ");
                                strcat(auxStr, buffer);
                                //if send the message to client fails 5 times, disconnect client from server.
                                if (send(clientArray[j].socket, auxStr, strlen(auxStr), 0) != strlen(auxStr))
                                {
                                    j--;
                                    sendToClientErrorsCnt++;
                                    if (sendToClientErrorsCnt == 5)
                                    {
                                        DisconnectClient(clientArray, j, address, addrlen);
                                    }
                                }
                                else
                                {
                                    sendToClientErrorsCnt = 0;
                                }
                            }
                        }
                    }
                }
                // std::cout << "Saiu da leitura" << std::endl;
            }
        }
    }

    return 0;
}

int FindClient(client* clientArray, int maxClients, const char * nomeUsuario){
    std::cout << "nomeUsuario = " << nomeUsuario << std::endl;
    for (int i = 0; i < maxClients; i++)
    {
        std::cout << "clientArray username = " << clientArray[i].username << std::endl;
        if(strcmp(clientArray[i].username, "") == 0){
            return -1;
        }
        if(strcmp(clientArray[i].username, nomeUsuario) == 0){
            return i;
        }
    }
    return -1;
}

void CreateChannel(channel *channelArray, int maxChannel, std::string newChannelName, client* newChannelAdmin)
{
    //Adiciona as informações do novo canal no array de canais e adicioana a pessoa que o criou como adiministrador.
    for(int i = 0; i < maxChannel; i++)
    {   
        if(channelArray[i].channelName == "")
        {
            channelArray[i].channelName = newChannelName;
            channelArray[i].admin = *newChannelAdmin;
            newChannelAdmin->isAdmin = true;
            break;
        }
    }
}

int InitServer(sockaddr_in *address, unsigned long *addrlen)
{
    int opt = TRUE;
    int serverSocket;
    // create a server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set server socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(serverSocket, (struct sockaddr *)address, sizeof(*address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the server socket
    if (listen(serverSocket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    *addrlen = sizeof(address);

    return serverSocket;
}



void DisconnectClient(client *clientArray, int pos, struct sockaddr_in address, int addrlen)
{
    getpeername(clientArray[pos].socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Host disconnected , ip %s , port %d \n",
           inet_ntoa(address.sin_addr),
           ntohs(address.sin_port));

    // Close the socket and mark as 0 in list for reuse
    close(clientArray[pos].socket);
    clientArray[pos].socket = 0;
}

int SetSockets(client *clientArray, int maxClients, int *serverSocket, fd_set *readfds)
{
    client cli;
    // clear the socket set
    FD_ZERO(readfds);

    // add master socket to set
    FD_SET(*serverSocket, readfds);
    int lastSocket = *serverSocket;

    // add child sockets to set
    for (int i = 0; i < maxClients; i++)
    {
        // socket descriptor
        cli.socket = clientArray[i].socket;

        // if valid socket descriptor then add to read list
        if (cli.socket > 0)
            FD_SET(cli.socket, readfds);

        // highest file descriptor number, need it for the select function
        if (cli.socket > lastSocket)
            lastSocket = cli.socket;
    }

    return lastSocket;
}

bool CheckForConnectionRequest(int *serverSocket, fd_set *readfds)
{
    // If something happened on the master socket, then its an incoming connection
    return FD_ISSET(*serverSocket, readfds);
}

void ConnectWithClient(client *clientArray, int maxClients, int *serverSocket, struct sockaddr_in address, int addrlen)
{
    client new_client;
    char buffer[MAX_MSG_LENGTH];
    char message[] = "ECHO Daemon v1.0 \r\n";

    if ((new_client.socket = accept(*serverSocket, (struct sockaddr *)&address,(socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // inform user of socket number - used in send and receive commands
    printf(
        "New connection , socket fd is %d , ip is : %s , port : %d\n",
        new_client.socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    // Username do novo client
    int valread = read(new_client.socket, buffer, MAX_MSG_LENGTH);
    buffer[valread] = '\0';
    strcpy(new_client.username, buffer);

    // send new connection greeting message
    if (send(new_client.socket, message, strlen(message), 0) != strlen(message))
    {
        perror("send");
    }

    puts("Welcome message sent successfully");

    // add new socket to array of sockets
    for (int i = 0; i < maxClients; i++)
    {
        // if position is empty
        if (clientArray[i].socket == 0)
        {
            clientArray[i].socket = new_client.socket;
            strcpy(clientArray[i].username, new_client.username);
            printf("Adding to list of sockets as %d\n", i);
            break;
        }
    }

    
}

bool CheckClientRequest(client cli, fd_set *readfds)
{
    return FD_ISSET(cli.socket, readfds);
}
