# Daniel Froés - 10255956
# Luana Balador Belisário - 10692245
# Pedro Henrique Nieuwenhoff - 10377729
all:
	g++ serverChat.cpp -o server.out
	g++ clientChat.cpp -o client.out

server: serverChat.cpp
	g++ serverChat.cpp -o server.out -std=c++11 -pthread
	./server.out

client: clientChat.cpp
	g++ clientChat.cpp -o client.out -std=c++11 -pthread
	./client.out