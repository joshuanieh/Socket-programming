#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <filesystem>
#include <sys/time.h>

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 1024
#define max_number_of_users 10
using namespace std;

int main(int argc, char const *argv[])
{	
	int o = true, j;
	ofstream ofile;
	ifstream ifile;
	char buff[max_number_of_users][buff_len], c;
	string filename[max_number_of_users];
	fd_set readfds;
	int client_fd, sockets[max_number_of_users] = {0}, max;
	string fileroot = "./server_dir";
	filesystem::create_directory(fileroot);
	const filesystem::path root{fileroot};
	cout << fileroot << endl;
	int port = atoi(argv[1]);
	cout << "port: " << port << endl;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = socket_domain;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	int socket_fd = socket(socket_domain, socket_type, socket_protocol);
	cout << "socket errno: " << errno << endl;
	cout << "bind returns: " << bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) << endl;
	cout << "bind errno: " << errno << endl;
	cout << "listen returns: " << listen(socket_fd, max_number_of_users) << endl;
	cout << "listen errno: " << errno << endl;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&o, sizeof(o));
	while(true){
		FD_ZERO(&readfds);
	    FD_SET(socket_fd, &readfds);
	    max = socket_fd;
        for(int i = 0; i < max_number_of_users; i++){
            memset(buff[i], '\0', buff_len);
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
            cout << "client_fd: " << client_fd << endl;
        }
        for(int i = 0; i < max_number_of_users; i++) {
            if(FD_ISSET(sockets[i], &readfds)) {
            	memset(buff[i], '\0', buff_len);
                if(recv(sockets[i], buff[i], 3, 0) <= 0) {
                    close(sockets[i]);
                    sockets[i] = 0;
                }
                else{
                	cout << buff[i] << endl;
                    if(strcmp(buff[i], "lsc") == 0) {
                    	// cout << root << endl;
						memset(buff[i], '\0', buff_len);
					    for (const auto &file: filesystem::directory_iterator{root}) {
					    	strcat(buff[i], file.path().filename().c_str());
					    	strcat(buff[i], "\n");
					    	// cout << file.path().filename().string() << endl;
					    }
					    // cout << buff << endl;
					    send(sockets[i], buff[i], buff_len, 0);
                    }
                    else if(strcmp(buff[i], "put") == 0) {
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                }
		                else {
		                	filename[i] = root/(string)buff[i];
		                	ofile.open(filename[i]);
		                	ofile.close();
		                	memset(buff[i], '\0', buff_len);
		                	while(true) {
				                if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
				                    close(sockets[i]);
				                    sockets[i] = 0;
				                }
				                else {
				                	cout << buff[i] << endl;
				                	if(strcmp(buff[i], "The file ends.") == 0) break;
		                			ofile.open(filename[i], ios_base::app);
				                	ofile << buff[i];
				                	ofile.close();
				                }
				            }
		                }
                    }
                    else if(strcmp(buff[i], "get") == 0) {
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                }
		                else {
		                	ifile.open(root/(string)buff[i]);
		                	if(ifile.is_open()) {
		                		send(sockets[i], "The file exists.", buff_len, 0);
		                		j = 0;
		                		memset(buff[i], '\0', buff_len);
		                		while(ifile.get(c) && j < buff_len) {
							    	buff[i][j] = c;
							    	j++;
							    	cout << "test" << endl;
							    }
							    cout << buff[i] << endl;
							    send(sockets[i], buff[i], buff_len, 0);
			                	ifile.close();
		                	}
			                else {
			                	send(sockets[i], ".", 16, 0);
			                }
		                }
                    }
                }
            }
        }
    }
	close(socket_fd);
	return 0;
}
