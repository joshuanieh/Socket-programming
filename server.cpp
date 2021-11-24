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
#define buff_len 2048
#define max_number_of_users 10
using namespace std;

int main(int argc, char const *argv[])
{	
	int o = true;
	ofstream file;
	char buff[buff_len];
	fd_set fds;
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
		FD_ZERO(&fds);
	    FD_SET(socket_fd, &fds);
	    max = socket_fd;
        for(int i = 0; i < max_number_of_users; i++){
            if(sockets[i] > 0) FD_SET(sockets[i], &fds);
            if(sockets[i] > max) max = sockets[i];
        }
        select(max + 1, &fds, NULL, NULL, NULL);
        if(FD_ISSET(socket_fd, &fds)){
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
            if(FD_ISSET(sockets[i], &fds)) {
                if(recv(sockets[i], buff, buff_len, 0) <= 0) {
                    close(sockets[i]);
                    sockets[i] = 0;
                }
                else{
                	// cout << buff << endl;
                    if(strcmp(buff, "ls") == 0) {
                    	// cout << root << endl;
						memset(buff, '\0', sizeof(buff));
					    for (const auto &file: filesystem::directory_iterator{root}) {
					    	strcat(buff, file.path().filename().c_str());
					    	strcat(buff, "\n");
					    	// cout << file.path().filename().string() << endl;
					    }
					    // cout << buff << endl;
					    send(sockets[i], buff, buff_len, 0);					    
                    }
                    else if(strcmp(buff, "put") == 0) {
                    	if(recv(sockets[i], buff, buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                }
		                else {
		                	file.open(root/(string)buff);
		                }
		                if(recv(sockets[i], buff, buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                }
		                else {
		                	cout << buff << endl;
		                	file << buff;
		                }
                    }
                }
            }
        }
    }








	// while(true){
	// 	int client_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
	// 	cout << "client_fd: " << client_fd << endl;
	// 	cout << "recv returns: " <<  << endl;
	// 	cout << "file name: " << fileroot + (string)buff << endl;
	// 	file.open(fileroot + '/' + (string)buff);
	// 	cout << "recv returns: " << recv(client_fd, buff, buff_len, 0) << endl;
	// 	cout << "content: " << buff << endl;
	// 	file << buff;
	// 	close(client_fd);
	// 	file.close();
	// }
	close(socket_fd);
	return 0;
}
