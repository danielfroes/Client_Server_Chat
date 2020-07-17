# Client_Server_Chat


Link Git:
https://github.com/danielfroes/Client_Server_Chat

Trabalho de redes - 1a entrega

Daniel Froés - 10255956<br>
Luana Balador Belisário - 10692245<br>
Pedro Henrique Nieuwenhoff - 10377729

##### COMO COMPILAR

1. Dentro da pasta _Client_Server_Chat_, abrir o terminal;
2. Compilar o programa servidor _serverChat.cpp_ usando no terminal o comando ```make server```;
3. Caso o servidor já esteva ativo, compilar o programa cliente _clientChat.cpp_ com outro terminal/outra aba/outro host para cada cliente que desejar entrar no **Chat** usando o comando ```make client``` no terminal;
4. Seguir as instruções abaixo para utilizar o chat :)

##### COMO UTILIZAR
Após o programa cliente ser executado de acordo com o passo 3 da seção anterior, o cliente deverá se conectar a um chat
1. O programa solicitará ao usuário digitar um nome de usuário:
   - O cliente deverá digitar seu nome de usuário através do comando ```/nickname``` e depois um ``` nomeUsuario```, por exemplo ```/nickname LuBalador```;
2. O programa solicitará ao usuário digitar o nome do chat que deseja entrar no formato ```/join #nome_do_canal``` ou ```/join &nome_do_canal```;
   - Ex: ```/join #RolezinhoSabadao```
   - Se o canal não existir, ele será criado e o usuário em questão será o administrador do canal;
3. Já no canal o usuário pode mandar mensagens aos outros usuários que também tiverem conectados a ele.

##### COMANDOS COMUNS A TODOS OS USUARIOS
- ```/join #nome_canal``` ou ```/join &nome_canal``` -> o usuário entra no canal especificado;
- ```/nickname apelido_desejado``` -> o usuário passa a ser reconhecido pelo apelido desejado;
- ```/ping``` -> o servidor retorna "pong", apenas para o usuário que o digitou;

##### COMANDOS APENAS PARA O ADMIN DO CANAL
- ```/kick nomeUsuario``` -> expulsa determinado usuário do canal;
  - o usuário expulso tem a opção de encerrar a conexão (```/quit```) ou entrar em algum canal novamente (```/join nomeCanal```);
- ```/mute nomeUsuario``` -> o usuário especificado fica mutado no canal, não podendo enviar mensagens aos outros usuários;
- ```/unmute nomeUsuario``` -> o usuário que estava mutado volta a poder enviar mensagens no canal;
- ```/whois nomeUsuario``` -> o admin recebe o endereço IP do usuário especificado.
