all: server/server
	@echo -n ""

server/server: server/server.o
	g++ $^ -o $@
server/server.o: server/server.cpp
	g++ -std=c++17 -c $^ -o $@

clean:
	rm -rf *.o server client server_dir client_dir
