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
#include <signal.h>

#define PORT 8080
#define MAX_MSG_LENGTH 2048
#define MAX_NICKNAME_LENGTH 50
#define MAX_CHANNELNAME_LENGTH 200

//** To do Cliente
void ChangeNickname(std::string *nickname);
bool CheckChannelName(std::string channelName);
void ReadMessage(int sock);
void SendMessage(std::string nickname, int sock);
int CreateSocket();
void ConnectToServer(int clientSock, std::string nickname);
void sigintHandler(int sig_num) ;
void ForceToJoinChannel(int sock);


int main(int argc, char const *argv[]) {



    /* Set the SIGINT (Ctrl-C) signal handler to sigintHandler
       Refer http://en.cppreference.com/w/c/program/signal */
    // signal(SIGINT, sigintHandler);
    std::string option;
    std::string nickname;


    // criando a socket do cliente
    int clientSock = CreateSocket();
    // while(option != "/nickname")
    // {
    //     std::cout << "Para criar um apelido, digite \"/nickname\"!" << std::endl;
    //     std::cin >> option;
    // }
    ChangeNickname(&nickname);

    std::cout << nickname << std::endl;

    // //only connects to server if the user types the right command
    // while(option != "/connect")
    // {
    //     std::cout << "Para conectar ao chat, digite \"/connect\"!" << std::endl;
    //     std::cin >> option;
    // }
    ConnectToServer(clientSock, nickname);
    std::cin.ignore();
    
    // definição execução das threads de envio e recebimento
    // de mensagens entre os clients e o server
    std::thread readThread(ReadMessage, clientSock);

    std::thread sendThread(SendMessage, nickname, clientSock);

    readThread.join();
    sendThread.join();

    return 0;
}


// func readMessage: Lê uma mensagem que foi enviada para o cliente
// @param: socket do cliente
void SendMessage(std::string nickname, int sock) {
    std::string msg;
    std::string msgAux;
    std::string msgDest;

    bool isInChannel = false;

    while (true) {

        if(!isInChannel)
        {
            ForceToJoinChannel(sock);
            isInChannel = true;

        }
        else
        {
            
            std::getline(std::cin, msg);

            //disconnect from the server and closes the application if /quit ou ctrl-d
            if(msg == "/quit" || std::cin.eof())
            {
                std::cout << "Até Logo! ^^" << std::endl;
                exit(0);
            }

            // faz o tratamento do tamanho da msg recebida pelo client e enviada
            // por esse. Caso a msg tenha mais que 2048 caracteres, ela é dividida
            // em mais de uma automaticamente
            for (int offset = 0; offset < msg.length(); offset += MAX_MSG_LENGTH) {
                msgAux = msg.substr(offset, MAX_MSG_LENGTH);
                // msgDest = nickname + ": " + msgAux + "\n";
                msgDest = msgAux + "\n";
                send(sock, msgDest.c_str(), msgDest.length(), 0);
            }
        }
    }
}

void ForceToJoinChannel(int sock)
{
    std::string option;
    std::string channelName;
    
    while (option.substr(0, 6) != "/join ")
    {
        std::cout << "Para entrar em um canal use o comando \"/join nome_do_canal\" \n";
        std::getline(std::cin, option);
        

        if(option.substr(0, 6) == "/join ")
        {
            channelName = option.substr(6, option.length()-6);
            
            if(option.length() == 6)
            {
                std::cout << "É necessário especificar um nome para o canal que deseja entrar. Tente novamente" << std::endl;
                option = "";
                continue;
            }

            if(!CheckChannelName(channelName))
            {
                option = "";
                continue;
            }
            
        }
    }

    //Mandando o comando \join  inteiro para o servidor
    send(sock, option.c_str(), option.length(), 0);
}

void ChangeNickname(std::string *nickname)
{
    printf("Digite um nome de usuário para entrar no chat: ");
    std::cin >> *nickname;
    while(nickname->length() > MAX_NICKNAME_LENGTH)
    {
        printf("\nO apelido digitado é maior que o limite de %d caracteres. Digite novamente um apelido: ", MAX_NICKNAME_LENGTH);
        std::cin >> *nickname;
    }
}

bool CheckChannelName(std::string channelName)
{

    char controlGChar[2];
    controlGChar[0] = 7;

    bool isValid = true;

    if(channelName[0] != '#' && channelName[0] != '&')
    {
        std::cout << "O nome do canal deve ser iniciado por \"#\" ou \"&\"." << std::endl;
        isValid = false;
    }

    if(channelName.find(",") != std::string::npos)
    {
        std::cout << "O nome do canal não pode conter \",\"." << std::endl;
        isValid = false;
    }

    if(channelName.find(controlGChar) != std::string::npos)
    {
        std::cout << "O nome do canal não pode conter \"^G\"." << std::endl;
        isValid = false;
    }
    
    if(channelName.length() > MAX_CHANNELNAME_LENGTH)
    {
        std::cout << "O nome do canal deve conter menos de " << MAX_CHANNELNAME_LENGTH << "characters."<< std::endl;
        isValid = false;
    }

    return isValid;
}
void JoinChannel()
{
    std::string channelName;
    std::cin >> channelName;

    //ler nome do canal
    //Mandar essa requisição para o servidor
}


// func readMessage: Lê uma mensagem que foi enviada para o cliente
// @param: socket do cliente
void ReadMessage(int sock) {
    char buffer[MAX_MSG_LENGTH + MAX_NICKNAME_LENGTH];
    int msgLength;

    while (true) {
        msgLength = read(sock, buffer, MAX_MSG_LENGTH + MAX_NICKNAME_LENGTH);
        buffer[msgLength] = '\0';
        printf("%s", buffer);
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
void ConnectToServer(int clientSock, std::string nickname) {
    int valread;
    char welcomeMsg[1000] = {0};
    struct sockaddr_in serv_addr;

    // configurando o endereço do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)  // Convert IPv4 and IPv6 addresses from text to binary form
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }

    // procura o servidor e conecta a socket
    if (connect(clientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(-1);
    }
    // Manda as requisições
    send(clientSock, nickname.c_str(), nickname.length(), 0);

    valread = read(clientSock, welcomeMsg, 1000);
    printf("%s\n", welcomeMsg);
}



/* Signal Handler for SIGINT */
void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    printf("\n Cannot be terminated using Ctrl+C \n");
    fflush(stdout);
}
