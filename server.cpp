#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <errno.h>

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 2048
using namespace std;

int main(int argc, char const *argv[])
{	
	ofstream file;
	char buff[buff_len];
	int port = atoi(argv[1]);
	cout << "port: " << port << endl;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	int socket_fd = socket(socket_domain, socket_type, socket_protocol);
	cout << "socket errno: " << errno << endl;
	cout << "bind returns: " << bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) << endl;
	cout << "bind errno: " << errno << endl;
	cout << "listen returns: " << listen(socket_fd, 10) << endl;
	cout << "listen errno: " << errno << endl;
	while(true){
		int client_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
		cout << "client_fd: " << client_fd << endl;
		recv(client_fd, buff, buff_len, 0);
		cout << buff << endl;
		file.open(buff);
		recv(client_fd, buff, buff_len, 0);
		cout << buff << endl;
		file << buff;
		close(client_fd);
		file.close();
	}
	close(socket_fd);
	return 0;
}
