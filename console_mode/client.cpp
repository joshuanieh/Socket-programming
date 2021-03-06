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
	string line, filename, command, name, passwd, friendName, option;
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
	string httpResponse, id = "";
	while ((true)) {
		//login
		while(true){
			cout << "Input your username: " << endl;
			getline(cin, name);
			cout << "Input your password: " << endl;
			getline(cin, passwd);
			line = "Login" + name + " " + passwd;
			strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
			strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
			// cout << httpRequest << endl;
			// cout << strlen(httpRequest) << endl;
			send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
			memset(buff, '\0', buff_len);
			recv(client_fd, buff, buff_len, 0);
			httpResponse = buff;
			// cout << buff << endl;
			id = httpResponse.substr(51, string::npos);
			// cout << id << endl;
			if(id != "x") break;
			cout << "Incorrect password, please try again." << endl;
		}

		cout << "\nWelcome, " << name << "!" << endl;

		while (true) {
			//menu
			cout << "\nHome" << endl;
			cout << "(1) List all friends" << endl;
			cout << "(2) Add a friend" << endl;
			cout << "(3) Delete a friend" << endl;
			cout << "(4) Choose a chat room" << endl;
			cout << "(5) Log out" << endl;

			getline(cin, option);
			num = 100;
			try {
				num = stoi(option);
			} catch (const std::exception&){
				cout << "";
			}
			if(num == 5) {
				cout << "Logged out successfully" << endl;
				break;
			}
			switch(num) {
				//List all friends
				case 1: {
					line = "List friends " + id;
					strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
					strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
					send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
					// cout << httpRequest << endl;
					memset(buff, '\0', buff_len);
					recv(client_fd, buff, buff_len, 0);
					// cout << buff << endl;
					httpResponse = buff;
					// cout << httpResponse << endl;
					if(httpResponse.substr(51, string::npos) == ""){
						cout << "No friend." << endl;
					}
					cout << httpResponse.substr(51, string::npos);
					break;
				}
				case 2: {
					cout << "Input a username: " << endl;
					getline(cin, friendName);
					line = "Add " + friendName + " " + id;
					strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
					strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
					// cout << httpRequest << endl;
					send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
					memset(buff, '\0', buff_len);
					recv(client_fd, buff, buff_len, 0);
					httpResponse = buff;
					// cout << httpResponse << endl;
					if(httpResponse.substr(51, string::npos) == "0"){
						cout << "Added successfully" << endl;
					}
					else if(httpResponse.substr(51, string::npos) == "1"){
						cout << "No user found" << endl;
					}
					break;
				}
				case 3: {
					cout << "Input a username: " << endl;
					getline(cin, friendName);
					line = "Remove " + friendName + " " + id;
					strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
					strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
					send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
					memset(buff, '\0', buff_len);
					recv(client_fd, buff, buff_len, 0);
					httpResponse = buff;
					// cout << httpResponse << endl;
					if(httpResponse.substr(51, string::npos) == "0"){
						cout << "Removed successfully" << endl;
					}
					else {
						cout << "No friend found" << endl;
					}
					break;
				}
				case 4: {
					while(true) {
						cout << "Choose a friend: (or \"quit\" to return)" << endl;
						cout << "Name: ";
						getline(cin, name);
						if(name == "quit") break;
						line = "Chat " + name + " " + id;
						strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
						strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
						send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
						memset(buff, '\0', buff_len);
						recv(client_fd, buff, buff_len, 0);
						httpResponse = buff;
						// cout << httpResponse << endl;
						if (httpResponse.substr(httpResponse.find("\r\n\r\n") + 4)[0] == '1') {
							cout << "invalid friend name" << endl;
						}
						else {
							cout << httpResponse.substr(httpResponse.find("\r\n\r\n") + 4) << endl;
							cout << "Please type \"Text <text>\" or \"Download <filename>\" or \"Upload <filename>\" (or \"quit\" to return)" << endl;
							while(true) {
								getline(cin, line);
								pos = line.find(" ");
								command = line.substr(0, pos);
								if(command == "quit") break;
								else if(command == "Text") {
									if(line.find(" ", pos + 1) != string::npos || pos == string::npos) {
										cout << "Command format error" << endl;
										continue;
									}
									line = "Text" + id + " " + line.substr(pos + 1);;
									strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
									strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
									send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									recv(client_fd, buff, buff_len, 0);
									memset(buff, '\0', buff_len);
									cout << "Text sent successfully" << endl;
								}
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
										strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
										send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
										recv(client_fd, buff, buff_len, 0);
										memset(buff, '\0', buff_len);
										
										while(file.peek() != EOF) {
											strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
											line = "FileImme" + id + " ";
											memset(buff, '\0', buff_len);
											file.read(buff, 3000 - line.size());
											// cout << buff << endl;
											if(strlen(buff) != 3000 - line.size()) break;
											// cout << "buff: " << buff << endl;
											line += buff;
											strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
											send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
											recv(client_fd, buff, buff_len, 0);
											memset(buff, '\0', buff_len);
										}
										strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
										line = "FileFinish" + id + " " + to_string(strlen(buff)) + " " + buff;
										char endZero[3000 - line.size()];
										memset(endZero, '-', 3000 - line.size());
										line += endZero;
										// cout << "line: " << line << endl;
										strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
										send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
										recv(client_fd, buff, buff_len, 0);
										memset(buff, '\0', buff_len);
										file.close();
										cout << "Upload " << filename << " successfully" << endl;
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
									strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
									send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									memset(buff, '\0', buff_len);
									recv(client_fd, buff, buff_len, 0);
									httpResponse = buff;
									if(httpResponse.substr(httpResponse.find("\r\n\r\n") + 4)[0] == 'x') {
										cout << "The " << filename << " doesn't exist" << endl;
										continue;
									}
									filesize = atoll((httpResponse.substr(httpResponse.find("\r\n\r\n") + 4)).c_str());
									file.open(root/filename, ios::out|ios::binary);
									file.close();
									for(long long l = 0; l < (filesize/4045); l++) {
										line = "DownloadImme" + id;
										strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
										strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
										send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
										memset(buff, '\0', buff_len);
										recv(client_fd, buff, buff_len, MSG_WAITALL);
										// cout << "buff: " << buff << endl;
										httpResponse = buff;
										line = httpResponse.substr(httpResponse.find("\r\n\r\n") + 4);
										file.open(root/filename, ios::app|ios::out|ios::binary);
										file.write(line.c_str(), 4045);
										file.close();
									}
									line = "DownloadImme" + id;
									strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
									strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
									send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
									memset(buff, '\0', buff_len);
									recv(client_fd, buff, filesize%4045 + 51, MSG_WAITALL);
									// cout << "buff: " << buff << endl;
									httpResponse = buff;
									line = httpResponse.substr(httpResponse.find("\r\n\r\n") + 4);
									file.open(root/filename, ios::app|ios::out|ios::binary);
									file.write(line.c_str(), filesize%4045);
									file.close();
									cout << "Download " << filename << " successfully" << endl;
								}
								else {
									cout << "Command not found" << endl;
								}
							}
						}
					}
					break;
				}
				default: {
					cout << "Invalid option, please try again." << endl;
					break;
				}
			}
		}
	}
	close(client_fd);
	return 0;
}
