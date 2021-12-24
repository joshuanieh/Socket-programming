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
#define buff_len 512
using namespace std;

int main(int argc, char const *argv[]) {
	char *ip;
	long long filesize;
	int port, pos, num;
	string line, filename, command, name;
	fstream file;
	streampos begin, end;
	string fileroot = "./client_dir";
	filesystem::create_directory(fileroot);
	const filesystem::path root{fileroot};
	const char *d = ":";
	ip = strtok((char *)argv[1], d);
	port = atoi(strtok(NULL, d));
	char buff[buff_len], c;
	struct sockaddr_in addr;
	int client_fd = socket(socket_domain, socket_type, socket_protocol);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));
	cout << "input your username:" << endl;
	while(getline(cin, name)) {
		send(client_fd, "logi", buff_len, MSG_NOSIGNAL);
		send(client_fd, name.c_str(), buff_len, MSG_NOSIGNAL);
		recv(client_fd, buff, buff_len, 0);
		cout << buff << endl;
		if(strcmp(buff, "connect successfully") == 0) break;
	}
	while(true) {
		getline(cin, line);
		pos = line.find(" ");
		command = line.substr(0, pos);
		if(command == "ls") {
			if(pos != string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			send(client_fd, "lsss", buff_len, MSG_NOSIGNAL);
			recv(client_fd, buff, buff_len, 0);
			cout << buff;
		}
		else if(command == "put") {
			if(line.find(" ", pos + 1) != string::npos || pos == string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			filename = line.substr(pos + 1);
		    file.open(root/filename, ios::in|ios::binary);
			if(file.is_open()) {
				begin = file.tellg();
				file.seekg(0, ios::end);
				end = file.tellg();
				send(client_fd, "putt", buff_len, MSG_NOSIGNAL);
				send(client_fd, filename.c_str(), buff_len, MSG_NOSIGNAL);
				send(client_fd, to_string(end - begin).c_str(), buff_len, MSG_NOSIGNAL);
				file.seekg(0, ios::beg);
				while(file.peek() != EOF) {
					// memset(buff, '\0', buff_len);
					// strcat(buff, "puti");
					send(client_fd, "puti", buff_len, MSG_NOSIGNAL);
					file.read(buff, buff_len);
					send(client_fd, buff, buff_len, MSG_NOSIGNAL);
				}
				file.close();
				cout << "put " << filename << " successfully" << endl;
			}
		  	else cout << "The " << filename << " doesn’t exist" << endl;
		}
		else if(command == "get") {
			if(line.find(" ", pos + 1) != string::npos || pos == string::npos) {
				cout << "Command format error" << endl;
				continue;
			}
			filename = line.substr(pos + 1);
			send(client_fd, "gett", buff_len, MSG_NOSIGNAL);
			send(client_fd, filename.c_str(), buff_len, MSG_NOSIGNAL);
			recv(client_fd, buff, buff_len, 0);;
			if(strcmp(buff, "The file exists.") != 0) {
				cout << "The " << filename << " doesn’t exist" << endl;
				continue;
			}
			recv(client_fd, buff, buff_len, 0);
			filesize = atoll(buff);
			file.open(root/filename, ios::out|ios::binary);
			file.close();
			for(long long l = 0; l < (filesize/buff_len); l++) {
				send(client_fd, "geti", buff_len, MSG_NOSIGNAL);
				recv(client_fd, buff, buff_len, 0);
				file.open(root/filename, ios::app|ios::out|ios::binary);
				file.write(buff, buff_len);
				file.close();
			}
			send(client_fd, "geti", buff_len, MSG_NOSIGNAL);
			recv(client_fd, buff, buff_len, 0);
			file.open(root/filename, ios::app|ios::out|ios::binary);
			file.write(buff, filesize%buff_len);
			file.close();
			cout << "get " << filename << " successfully" << endl;
		}
		else {
			cout << "Command not found" << endl;
		}
	}
	close(client_fd);
	return 0;
}