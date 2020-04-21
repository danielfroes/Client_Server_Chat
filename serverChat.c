// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080


int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket[2], valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char *hello = "Hello from server"; 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	 if( setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    } 
    
	address.sin_family = AF_INET; //fala que é o protocolo IPv4
	address.sin_addr.s_addr = INADDR_ANY; //conecta o servidor ao host local
	address.sin_port = htons( PORT );  //seta o edereço da PORT
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	//Coloca o servidor num estado passivo e deixa a sua fila de clientes com tamanho de 3
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	puts("Waiting for connections ...");
	//espera dois clientes chamarem
	for(int i = 0; i < 2; i++)
	{
		
		if ((new_socket[i] = accept(server_fd, (struct sockaddr *)&address, 
						(socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}
		
	}	

	printf("2 Clientes conectados");


	valread = read( new_socket[0] , buffer, 1024); 
	

	printf("mensagem recebida\n" ); 


	send(new_socket[1] , buffer , strlen(buffer) , 0 ); 

	printf("Hello message sent\n");
     
	return 0; 
} 
