all:
	gcc serverChat.c -o server.out
	gcc clientChat.c -o client.out

server: server.out serverChat.c
	gcc serverChat.c -o server.out
	./server.out

client: client.out clientChat.c
	gcc clientChat.c -o client.out
	./client.out