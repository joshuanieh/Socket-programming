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
	
	//login
	while(true){
		cout << "input your username: " << endl;
		getline(cin, name);
		cout << "input your password: " << endl;
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

		getline(cin, option);
		num = 5;
		try {
			num = stoi(option);
		} catch (const std::exception&){
			cout << "";
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
				cout << httpResponse.substr(51, string::npos);
				break;
			}
			case 2: {
				cout << "input a username: " << endl;
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
				line = "Remove " + friendName + " " + id;
				strcpy(httpRequest, "POST / HTTP/1.1\r\nContent-Length: ");
				strcat(httpRequest, (to_string(line.size()) + "\r\n\r\n" + line).c_str());
				send(client_fd, httpRequest, strlen(httpRequest), MSG_NOSIGNAL);
				memset(buff, '\0', buff_len);
		