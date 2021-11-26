all: server client
	@echo -n ""

server: server.o
	g++ $^ -o $@
client: client.o
	g++ $^ -o $@
server.o: server.cpp
	g++ -std=c++17 -c $^ -o $@
client.o: client.cpp
	g++ -std=c++17 -c $^ -o $@

clean:
	rm -rf *.o server client server_dir client_dir