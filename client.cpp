#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <errno.h>
#include <filesystem>

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 1024
using namespace std;

int main(int argc, char const *argv[]) {
	bool flag;
	char *ip;
	int port, i;
	string filename, command;
	ifstream ifile;
	ofstream ofile;
	string fileroot = "./client_dir";
	filesystem::create_directory(fileroot);
	const filesystem::path root{fileroot};
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
	while(true) {
		flag = true;
		cin >> command;
		if(command == "ls") {
			send(client_fd, "lsc", 3, 0);
			recv(client_fd, buff, buff_len, 0);
			cout << buff;
		}
		else if(command == "put") {
			cin >> filename;
			// cin.getline(buff, buff_len);
		    ifile.open(filename);
			if(ifile.is_open()) {
				send(client_fd, "put", 3, 0);
				send(client_fd, filename.c_str(), buff_len, 0);
				while(flag) {
					i = 0;
					memset(buff, '\0', buff_len);
					while(i < buff_len) {
				    	if(ifile.get(c)) {
					    	buff[i] = c;
					    	i++;
					    }
					    else {
					    	flag = false;
					    	break;
					    }
				    }
				    cout << buff << endl;
					send(client_fd, buff, buff_len, 0);
				}
				send(client_fd, "The file ends.", buff_len, 0);
				cout << "put " << filename << " successfully" << endl;
				ifile.close();
			}
		  	else cout << "The " << filename << " doesn’t exist" << endl;
		}
		else if(command == "get") {
			cin >> filename;
			// cin.getline(buff, buff_len);
			send(client_fd, "get", 3, 0);
			send(client_fd, filename.c_str(), filename.size(), 0);
			recv(client_fd, buff, buff_len, 0);
			cout << buff << endl;
			if(strcmp(buff, "The file exists.") == 0) {
				memset(buff, '\0', buff_len);
				recv(client_fd, buff, buff_len, 0);
				cout << buff << endl;
		    	ofile.open(root/filename);
				ofile << buff;
				cout << "get " << filename << " successfully" << endl;
				ofile.close();
			}
		  	else cout << "The " << filename << " doesn’t exist" << endl;
		}
		else {
			cout << "Command not found" << endl;
		}
	}
	close(client_fd);
	return 0;
}