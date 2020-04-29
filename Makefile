all:
	g++ serverChat.cpp -o server.out
	g++ clientChat.cpp -o client.out

server: server.out serverChat.cpp
	g++ serverChat.cpp -o server.out -std=c++11 -pthread
	./server.out

client: client.out clientChat.cpp
	g++ clientChat.cpp -o client.out -std=c++11 -pthread
	./client.out 