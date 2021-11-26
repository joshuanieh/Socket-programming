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
	int port, i, pos;
	string line, filename, command, name;
	fstream file;
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
	cout << "input your username:" << endl;
	while(getline(cin, name)) {
		send(client_fd, "logi", buff_len, 0);
		send(client_fd, name.c_str(), buff_len, 0);
		recv(client_fd, buff, buff_len, 0);
		cout << buff << endl;
		if(strcmp(buff, "connect successfully") == 0) break;
	}
	while(true) {
		flag = true;
		getline(cin, line);
		pos = line.find(" ");
		command = line.substr(0, pos);
		if(command == "ls") {
			if(pos != string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			send(client_fd, "lsss", buff_len, 0);
			recv(client_fd, buff, buff_len, 0);
			cout << buff;
		}
		else if(command == "put") {
			if(line.find(" ", pos + 1) != string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			filename = line.substr(pos + 1);
			send(client_fd, "putt", buff_len, 0);
			send(client_fd, filename.c_str(), buff_len, 0);
			// cin.getline(buff, buff_len);
		    file.open(root/filename, ios::in);
			if(file.is_open()) {
				while(flag) {
					i = 0;
					memset(buff, '\0', buff_len);
					while(i < buff_len) {
				    	if(file.get(c)) {
					    	buff[i] = c;
					    	i++;
					    	// cout << "errno: " << c << endl;
					    }
					    else {
					    	flag = false;
					    	break;
					    }
				    }
				    // cout << buff << endl;
					send(client_fd, "puti", buff_len, 0);
					send(client_fd, buff, buff_len, 0);
				}
				send(client_fd, "puti", buff_len, 0);
				send(client_fd, "The file ends.", buff_len, 0);
				send(client_fd, to_string(buff_len - i).c_str(), buff_len, 0);
				// cout <<  to_string(buff_len - i).c_str() << endl;
				cout << "put " << filename << " successfully" << endl;
				file.close();
			}
		  	else cout << "The " << filename << " doesn’t exist" << endl;
		}
		else if(command == "get") {
			if(line.find(" ", pos + 1) != string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			filename = line.substr(pos + 1);
			// cin.getline(buff, buff_len);
			send(client_fd, "gett", buff_len, 0);
			send(client_fd, filename.c_str(), buff_len, 0);
			recv(client_fd, buff, buff_len, 0);
			if(strcmp(buff, "The file exists.") != 0) {
				cout << "The " << filename << " doesn’t exist" << endl;
				continue;
			}
			file.open(root/filename, ios::out);
			file.close();
			while(true) {
		        memset(buff, '\0', buff_len);
				send(client_fd, "geti", buff_len, 0);
				recv(client_fd, buff, buff_len, 0);
				cout << buff << endl;
				if(strcmp(buff, "The file ends.") == 0) {
					recv(client_fd, buff, buff_len, 0);
					cout << buff << endl;
            		char buffer[atoi(buff)];
            		file.open(root/filename, ios::in);
            		file.read(buffer, sizeof(buffer));
                	file.close();
                	file.open(root/filename, ios::out);
                	file.write(buffer, sizeof(buffer));
                	file.close();
					break;
				}
		    	file.open(root/filename, ios::out|ios::app);
				file.write(buff, sizeof(buff));
				file.close();
			}
			cout << "get " << filename << " successfully" << endl;
		}
		else {
			cout << "Command not found" << endl;
		}
	}
	close(client_fd);
	return 0;
}