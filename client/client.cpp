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
#define buff_len 4096
using namespace std;

int main(int argc, char const *argv[]) {
	//declarations
	char *ip;
	char cnd[20];
	long long filesize;
	int port, pos, num;
	string line, filename, command, name, passwd, friendName;
	fstream file;
	streampos begin, end;
	string fileroot = "./client_dir";
	filesystem::create_directory(fileroot);
	const filesystem::path root{fileroot};
	const char *d = ":";
	ip = strtok((char *)argv[1], d);
	port = atoi(strtok(NULL, d));
	char buff[buff_len], c;

	//socket
	struct sockaddr_in addr;
	int client_fd = socket(socket_domain, socket_type, socket_protocol);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	connect(client_fd, (struct sockaddr*)&addr, sizeof(addr));
	char httpRequest[] = "POST / HTTP/1.1\r\nContent-Length: ";//+ {number}/r/n/r/n{data}
	string httpResponse, id;
	
	//login
	while(true){
		cout << "input your username: " << endl;
		getline(cin, name);
		cout << "input your password: " << endl;
		getline(cin, passwd);
		sprintf(cnd, "%d/r/n/r/nLogin%s %s", name.size() + passwd.size() + 6, name, passwd);
		strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
		strcat(httpRequest, cnd);
		send(client_fd, httpRequest, sizeof(httpRequest), MSG_NOSIGNAL);
		recv(client_fd, buff, buff_len, MSG_WAITALL);
		httpResponse = buff;
		id = httpResponse.substr(51, string::npos);
		cout << id << endl;
		if(id != "x") break;
	}

	//menu
	cout << "Home" << endl;
	cout << "(1) List all friends" << endl;
	cout << "(2) Add friend" << endl;
	cout << "(3) Delete friend" << endl;
	cout << "(4) Choose a chat room" << endl;

	while (true) {
		cin >> num;
		switch(num) {
			//List all friends
			case 1: {
				sprintf(cnd, "%d/r/n/r/nList friends %s", id.size() + 13, id);
				strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
				strcat(httpRequest, cnd);
				send(client_fd, httpRequest, sizeof(httpRequest), MSG_NOSIGNAL);
				recv(client_fd, buff, buff_len, MSG_WAITALL);
				httpResponse = buff;
				cout << httpResponse.substr(51, string::npos);
				break;
			}
			case 2: {
				cout << "input a username: " << endl;
				getline(cin, friendName);
				sprintf(cnd, "%d/r/n/r/nAdd %s %s", friendName.size() + id.size() + 5, friendName, id);
				strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
				strcat(httpRequest, cnd);
				send(client_fd, httpRequest, sizeof(httpRequest), MSG_NOSIGNAL);
				recv(client_fd, buff, buff_len, MSG_WAITALL);
				httpResponse = buff;
				if(httpResponse.substr(51, string::npos) == "0"){
					cout << "added successfully" << endl;
				}
				else if(httpResponse.substr(51, string::npos) == "1"){
					cout << "no user found" << endl;
				}
				break;
			}
			case 3: {
				cout << "input a username: " << endl;
				getline(cin, friendName);
				sprintf(cnd, "%d\r\n\r\nRemove %s %s", friendName.size() + id.size() + 8, friendName, id);
				strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
				strcat(httpRequest, cnd);
				send(client_fd, httpRequest, sizeof(httpRequest), MSG_NOSIGNAL);
				recv(client_fd, buff, buff_len, MSG_WAITALL);
			}
			case 4: {
				while(true) {
					cout << "Choose a friend: (or \"quit\" to return)" << endl;
					cout << "Name: ";
					getline(cin, name);
					if(name == "quit") break;
					line = "Chat " + name + " " + id;
					strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
					strcat(httpRequest, to_string(line.size()) + "\r\n\r\n" + line);
					send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
					recv(client_fd, buff, buff_len, MSG_WAITALL);
					httpResponse = buff;
					if (httpResponse.substr(httpResponse.find("\r\n\r\n") + 4)[0] === '1') {
						cout << "invalid friend name" << endl;
					}
					else {
						cout << "Please type \"Text ...\" or \"Download ...\" or \"Upload ...\" (or \"quit\" to return)" << endl;
						while(true) {
							getline(cin, line);
							pos = line.find(" ");
							command = line.substr(0, pos);
							if(command == "quit") break;
							else if(command == "Upload") {
								if(line.find(" ", pos + 1) != string::npos || pos == string::npos) {
									cout << "Command format error" << endl;
									continue;
								}
								filename = line.substr(pos + 1);
							    file.open(root/filename, ios::in|ios::binary);
								if(file.is_open()) {
									line = "FileName" + id + " " + filename;
									strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
									strcat(httpRequest, to_string(line.size()) + "\r\n\r\n" + line);
									send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									
									while(file.peek() != EOF) {
										strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
										line = "FileImme" + id + " ";
										file.read(buff, 3000 - line.size());
										if(strlen(buff) != 3000 - line.size()) break;
										line += buff;
										strcat(httpRequest, to_string(line.size()) + "\r\n\r\n" + line);
										send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									}
									strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
									line = "FileFinish" + id + " " + to_string(strlen(buff)) + " " + buff;
									strcat(httpRequest, to_string(line.size()) + "\r\n\r\n" + line);
									send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									file.close();
									cout << "upload " << filename << " successfully" << endl;
								}
							  	else cout << "The " << filename << " doesn't exist" << endl;
							}
							else if(command == "Download") {
								if(line.find(" ", pos + 1) != string::npos || pos == string::npos) {
									cout << "Command format error" << endl;
									continue;
								}
								filename = line.substr(pos + 1);
								line = "Download" + id + " " + filename;
								strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
								strcat(httpRequest, to_string(line.size()) + "\r\n\r\n" + line);
								send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
								
								if(strcmp(buff, "The file exists.") != 0) {
									cout << "The " << filename << " doesn't exist" << endl;
									continue;
								}
								recv(client_fd, buff, buff_len, MSG_WAITALL);
								filesize = atoll(buff);
								file.open(root/filename, ios::out|ios::binary);
								file.close();
								for(long long l = 0; l < (filesize/buff_len); l++) {
									sprintf(cnd, "geti");
									send(client_fd, cnd, 5, MSG_NOSIGNAL);
									recv(client_fd, buff, buff_len, MSG_WAITALL);
									file.open(root/filename, ios::app|ios::out|ios::binary);
									file.write(buff, 4045);
									file.close();
								}
								sprintf(cnd, "geti");
								send(client_fd, cnd, 5, MSG_NOSIGNAL);
								recv(client_fd, buff, buff_len, MSG_WAITALL);
								file.open(root/filename, ios::app|ios::out|ios::binary);
								file.write(buff, filesize%buff_len);
								file.close();
								cout << "get " << filename << " successfully" << endl;
							}
							else {
								cout << "Command not found" << endl;
							}
						}
					}
				}
			}
		}
	}
	close(client_fd);
	return 0;
}
