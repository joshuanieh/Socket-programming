all: server/server
	@echo -n ""

server/server: server/server.o
	g++ $^ -o $@ -lcrypto
server/server.o: server/server.cpp
	g++ -std=c++17 -c $^ -o $@

clean:
	rm -rf *.o server/server server/server.o server_dir client_dir
