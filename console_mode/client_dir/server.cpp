#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <filesystem>
#include <sys/time.h>
#include <algorithm>
#include <vector>
#include <unordered_map>

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 4096
#define max_number_of_users 10
#define max_number_of_users_in_database INT16_MAX
using namespace std;

int main(int argc, char const *argv[]) {
	string httpRequest, fileroot = "./public", name, pass;
	const filesystem::path root{fileroot};
	vector<string> filelist, allUsername;
	string chattingFriend[max_number_of_users_in_database], filename[max_number_of_users_in_database];
	int filesize[max_number_of_users_in_database], headerLength;
	for (const auto &n : filesystem::directory_iterator{root}) {
		if (n.is_directory())
            allUsername.push_back(n.path().stem().string());
	}
	unordered_map <string, string> userPasswordPairs;
	fstream file;
	file.open("./password.txt", ios::in);
	while(file >> name >> pass) userPasswordPairs[name] = pass;
	file.close();

	streampos begin, end;
	streampos base[max_number_of_users_in_database], chatBase[max_number_of_users_in_database];
	bool flag, fileFlag = false;
	int o = true, j, index;
	// long long filesize[max_number_of_users];
	char buff[buff_len], c, httpResponse[buff_len];
	string data;
	fd_set readfds;
	int client_fd, sockets[max_number_of_users] = {0}, max, contentLengthPos, contentLength;
	int port = atoi(argv[1]);
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	int socket_fd = socket(socket_domain, socket_type, socket_protocol);
	bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
	listen(socket_fd, max_number_of_users);
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&o, sizeof(o));
    filesystem::create_directory(root);
	while(true){
		cout << "============================================================" << endl;
		FD_ZERO(&readfds);
	    FD_SET(socket_fd, &readfds);
	    max = socket_fd;
        for(int i = 0; i < max_number_of_users; i++){
            memset(buff, '\0', buff_len);
            if(sockets[i] > 0) {
            	FD_SET(sockets[i], &readfds);
            }
            if(sockets[i] > max) max = sockets[i];
        }
        select(max + 1, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(socket_fd, &readfds)){
            client_fd = accept(socket_fd, (struct sockaddr *)NULL, (socklen_t *)NULL);
            for(int i = 0; i < max_number_of_users; i++){
                if(sockets[i] == 0){
                    sockets[i] = client_fd;
                    break;
                }
            }
        }
        for(int i = 0; i < max_number_of_users; i++) {
            if(FD_ISSET(sockets[i], &readfds)) {
            	if(fileFlag) {
            		if(recv(sockets[i], buff, headerLength + 3000 + 2, MSG_WAITALL) <= 0) {
	                    close(sockets[i]);
	                    sockets[i] = 0;
	                }
	                else {
	                	cout << sockets[i] << endl;
	                	
	                	flag = false;
	                	httpRequest = buff;
						// cout << "buffer!!: " << buff << endl;
	                	// print out http request
	                	// for (int i = 0; i < httpRequest.size(); ++i) {
	                	// 	if(httpRequest[i] == '\r') cout << "\\r";
	                	// 	else if(httpRequest[i] == '\n') cout << "\\n";
	                	// 	else cout << httpRequest[i];
	                	// }
	                	cout << "httpRequest: " << httpRequest << endl;

	            		// contentLengthPos = httpRequest.find("Content-Length: ");
	            		// contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
						data = httpRequest.substr(httpRequest.find("\r\n\r\n")