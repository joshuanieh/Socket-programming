all: server ../client/client
	@echo -n ""

server: server.o
	g++ $^ -o $@
server.o: server.cpp
	g++ -std=c++17 -c $^ -o $@

../client/client: ../client/client.o
	g++ $^ -o $@
../client/client.o: ../client/client.cpp
	g++ -std=c++17 -c $^ -o $@

clean:
	rm -rf server server.o

clean_all:
	rm -rf server server.o ../client/client_dir/ public/* password.txt
