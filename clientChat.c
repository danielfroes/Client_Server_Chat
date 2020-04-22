// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 8080 
   
int main(int argc, char const *argv[]) 
{ 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char msg[1024];
    char buffer[1024] = {0}; 

    //criando a socket do cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

    //configurando o endereço do servidor
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)   // Convert IPv4 and IPv6 addresses from text to binary form
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

    valread = read(sock , buffer, 1024); 
    printf("%s\n",buffer );  
    int opc;
    char tmp[1024];
    
    while(1){
        scanf("%d", &opc);
        if(opc == 1){
            scanf("\n%[^\n]s", msg);
            send(sock , msg , strlen(msg) , 0 ); 
        }        
        else{
            valread = read(sock ,tmp, 1024);
            tmp[valread] = '\0';
            printf("%s\n",tmp );
        }
        
    }
    return 0; 
} 