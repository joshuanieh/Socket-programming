all: server client
	@echo -n ""

server: server.o
	g++ $^ -o $@
client: client.o
	g++ $^ -o $@
server.o: server.cpp
	g++ -c $^ -o $@
client.o: client.cpp
	g++ -c $^ -o $@