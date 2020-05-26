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

#define TRUE 1
#define FALSE 0
#define PORT 8080

#define MAX_USERS_NUMBER 100
#define MAX_MSG_LENGTH 2048


//** To do Servidor
// - Servidor detectar os cliente que não estão recebendo mensagem e apos 5 tentativas, desconectar o cliente

typedef struct _client
{
    int socket;
    char username[MAX_MSG_LENGTH];
} client;

void InitServer()
{
    
}

void DisconnectClient( client* clientArray, int pos, struct sockaddr_in address, int addrlen)
{
    getpeername(clientArray[pos].socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Host disconnected , ip %s , port %d \n",
    inet_ntoa(address.sin_addr),
    ntohs(address.sin_port));

    // Close the socket and mark as 0 in list for reuse
    close(clientArray[pos].socket);
    clientArray[pos].socket = 0;
}



int main(int argc, char *argv[]) {
    int opt = TRUE;
    int master_socket, addrlen,  max_clients = MAX_USERS_NUMBER, activity, i, valread, max_sd,  cnt_errors = 0;
    
    client client_socket[MAX_USERS_NUMBER], new_client, sd;

    struct sockaddr_in address;

    char buffer[MAX_MSG_LENGTH], auxStr[MAX_MSG_LENGTH];  // data buffer of 1K

    // set of socket descriptors
    fd_set readfds;

    // a message
    char message[] = "ECHO Daemon v1.0 \r\n";





    // initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) {
        client_socket[i].socket = 0;
        // client_socket[i].username;
    }

    // create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (TRUE) {
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++) {
            // socket descriptor
            sd.socket = client_socket[i].socket;

            // if valid socket descriptor then add to read list
            if (sd.socket > 0) FD_SET(sd.socket, &readfds);

            // highest file descriptor number, need it for the select function
            if (sd.socket > max_sd) max_sd = sd.socket;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket,
                     &readfds))  // checa se aconteceu alguma no master socket
        {
            if ((new_client.socket = accept(master_socket, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            printf(
                "New connection , socket fd is %d , ip is : %s , port : %d\n",
                new_client.socket, inet_ntoa(address.sin_addr),
                ntohs(address.sin_port));



            valread = read(new_client.socket, buffer, MAX_MSG_LENGTH);
            buffer[valread] = '\0';
            strcpy(new_client.username, buffer);
            

           

            // send new connection greeting message
            if (send(new_client.socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            // add new socket to array of sockets
            for (i = 0; i < max_clients; i++) {
                // if position is empty
                if (client_socket[i].socket == 0) {
                    client_socket[i].socket = new_client.socket;
                    strcpy(client_socket[i].username, new_client.username);
                    printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd.socket, &readfds)) {
                // Check if it was for closing, and also read the
                // incoming message
                if ((valread = read(sd.socket, buffer, MAX_MSG_LENGTH)) == 0) {
                    // // Somebody disconnected , get his details and print
                    // getpeername(sd.socket, (struct sockaddr *)&address,
                    //             (socklen_t *)&addrlen);
                    // printf("Host disconnected , ip %s , port %d \n",
                    //        inet_ntoa(address.sin_addr),
                    //        ntohs(address.sin_port));

                    // // Close the socket and mark as 0 in list for reuse
                    // close(sd.socket);
                    // client_socket[i].socket = 0;
                    DisconnectClient(client_socket, i, address, addrlen);
                }

                // Echo back the message that came in to other clients
                else {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';
                
                    if(strcmp(buffer, "/ping\n") == 0)
                    {
                        strcpy(buffer, "pong\n");
                        std::cout << buffer << std::endl;
                        send(sd.socket, buffer, strlen(buffer), 0);    
                    }
                    else
                    {   
                        for (int j = 0; j < max_clients; j++)
                        {   
                            if(client_socket[j].socket != 0)
                            {
                                
                                strcpy(auxStr, sd.username); 
                                strcat(auxStr, ": ");
                                strcat(auxStr, buffer);

                                //aqui
                                if(send(client_socket[j].socket, auxStr, strlen(auxStr), 0) != strlen(auxStr))
                                {   
                                    j--;
                                    cnt_errors++;
                                    std::cout << "contador de erro: " << cnt_errors << std::endl;
                                }
                                else{
                                    cnt_errors = 0;

                                    std::cout << "contador de resetado" << std::endl;
                                }
                                if(cnt_errors == 5)
                                {
                                    DisconnectClient(client_socket, j, address, addrlen);
                                }
                            }
                        }
                    }
                }
            }
        }
    }




    return 0;
}
