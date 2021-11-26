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
	long base[max_number_of_users];
	bool flag;
	int o = true, j, count[max_number_of_users];
	fstream file[max_number_of_users];
	char buff[max_number_of_users][buff_len], c;
	string filename[max_number_of_users], username[max_number_of_users] = {""};
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
                if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
                    close(sockets[i]);
                    sockets[i] = 0;
                    username[i] = "";
                }
                else{
                	// cout << buff[i] << endl;
                	flag = true;
                    if(strcmp(buff[i], "lsss") == 0) {
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
                    else if(strcmp(buff[i], "putt") == 0) {
                    	count[i] = 0;
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                    username[i] = "";
		                }
		                else {
		                	filename[i] = root/(string)buff[i];
		                	file[i].open(filename[i], ios::out);
		                	file[i].close();
		                }
                    }
                    else if(strcmp(buff[i], "puti") == 0) {
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                    username[i] = "";
		                }
		                else {
			                if(strcmp(buff[i], "The file ends.") == 0) {
			                	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
				                    close(sockets[i]);
				                    sockets[i] = 0;
				                    username[i] = "";
				                }
			                	else {
			                		count[i] *= buff_len;
			                		count[i] -= atoi(buff[i]);
			                		cout << 1 << endl;
			                		cout << count[i] << endl;
			                		char buffer[count[i]];
			                		file[i].open(filename[i], ios::in);
			                		file[i].read(buffer, sizeof(buffer));
				                	file[i].close();
				                	file[i].open(filename[i], ios::out);
				                	file[i].write(buffer, sizeof(buffer));
				                	file[i].close();
			                	}
			                }
			                else {
	                			file[i].open(filename[i], ios::app|ios::out);
			                	file[i].write(buff[i], sizeof(buff[i]));
			                	file[i].close();
			                	count[i]++;
			                }
			                // while(file[sizeof(file) - 1] == '\0')
		                }
                    }
                    else if(strcmp(buff[i], "gett") == 0) {
                    	base[i] = 0;
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                    username[i] = "";
		                }
		                else {
		                	filename[i] = root/(string)buff[i];
		                	file[i].open(filename[i], ios::in);
		                	if(file[i].is_open()) {
		                		send(sockets[i], "The file exists.", buff_len, 0);
		                		file[i].close();
		                	}
			                else send(sockets[i], "The file doesn't exists.", buff_len, 0);
		                }
                    }
                    else if(strcmp(buff[i], "geti") == 0) {
                    	memset(buff[i], '\0', buff_len);
                		j = 0;
	                	file[i].open(filename[i], ios::in);
                		file[i].seekg(base[i], ios::beg);
                		while(file[i].get(c) && j < buff_len) {
					    	buff[i][j] = c;
					    	j++;
					    }
					    file[i].close();
					    base[i] += j;
					    cout << buff[i] << endl;
					    send(sockets[i], buff[i], buff_len, 0);
	                	if(j != buff_len) {
	                		if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
			                    close(sockets[i]);
			                    sockets[i] = 0;
			                    username[i] = "";
			                }
			                else {
			                	send(sockets[i], "The file ends.", buff_len, 0);
						    	cout << base[i];
				                send(sockets[i], to_string(base[i]).c_str(), buff_len, 0);
			                }
	                	}
                    }
                    else if(strcmp(buff[i], "logi") == 0) {
                    	memset(buff[i], '\0', buff_len);
                    	if(recv(sockets[i], buff[i], buff_len, 0) <= 0) {
		                    close(sockets[i]);
		                    sockets[i] = 0;
		                    username[i] = "";
		                }
                    	for(int k = 0; k < max_number_of_users; k++) {
                    		if(strcmp(buff[i], username[k].c_str()) == 0) {
                    			send(sockets[i], "username is in used, please try another", buff_len, 0);
                    			flag = false;
                    			break;
                    		}
                    	}
                    	if(!flag) continue;
                    	send(sockets[i], "connect successfully", buff_len, 0);
                    	username[i] = buff[i];
                    }
                }
            }
        }
    }
	close(socket_fd);
	return 0;
}
