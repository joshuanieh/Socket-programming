#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <errno.h>

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 2048
using namespace std;

int main(int argc, char const *argv[]) {	
	char *ip;
	int port, i = 0;
	string filename;
	ifstream file;
	const char *d = ":";
	ip = strtok((char *)argv[1], d);
	port = atoi(strtok(NULL, d));
	cout << "port: " << port << endl;
	cout << "ip: " << ip << endl;
	char buff[buff_len], c;
	struct sockaddr_in addr;
	int client_fd = socket(socket_domain, socket_type, socket_protocol);
	cout << "socket errno: " << errno << endl;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	cout << "connect returns: " << connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) << endl;
	cout << "connect errno: " << errno << endl;
	cout << "Going to send some file?" << endl;
	cin >> filename;
	// cin.getline(buff, buff_len);
	file.open(filename);
	if(file.is_open()) {
	    while(file.get(c) && i < buff_len) {
	    	buff[i] = c;
	    	i++;
	    }
	    cout << buff << endl;
	  }
  	else cout << "Unable to open file" << endl;
	file.close();
	cout << "send returns: " << send(client_fd, filename.c_str(), buff_len, 0) << endl;
	cout << "send returns: " << send(client_fd, buff, i - 1, 0) << endl;
	cout << "send errno: " << errno << endl;
	close(client_fd);
	return 0;
}