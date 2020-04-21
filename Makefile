all:
	gcc serverChat.c -o server
	gcc clientChat.c -o client

server: server.out serverChat.c
	gcc serverChat.c -o server
	./server

client: client.out clientChat.c
	gcc clientChat.c -o client
	./client