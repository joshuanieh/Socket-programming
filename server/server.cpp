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

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 512
#define max_number_of_users 10
using namespace std;

int main(int argc, char const *argv[]) {	
	vector<string> filelist;
	streampos begin, end, base[max_number_of_users];
	bool flag;
	char httpMessage[4096] = "";
	int o = true, j;
	fstream file;
	long long filesize[max_number_of_users];
	char buff[buff_len], c;
	string filename[max_number_of_users], username[max_number_of_users] = {""};
	fd_set readfds;
	int client_fd, sockets[max_number_of_users] = {0}, max;
	string fileroot = "./server_dir";
	filesystem::create_directory(fileroot);
	const filesystem::path root{fileroot};
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
	while(true){
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
                if(recv(sockets[i], buff, 6, MSG_WAITALL) <= 0) {
                    close(sockets[i]);
                    sockets[i] = 0;
                    username[i] = "";
                }
                else{
                	strcat(httpMessage, buff);
                	size_t len = strlen(httpMessage);
                	if(httpMessage[len - 1] == '\n' && httpMessage[len - 2] == '\r') {
                		cout << httpMessage << endl;
                		strcpy(httpMessage, "");
                	}
                	// cout << buff << endl;
                	// flag = true;
        //         	if(strcmp(buff, "lsaf") == 0) {
		      //           for (const auto &n: filesystem::directory_iterator{root/username[i]}) filelist.push_back(n.path().filename().string());
					   //  sort(filelist.begin(), filelist.end());
					   //  for (const string &name: filelist) {
					   //  	strcat(buff, name.c_str());
					   //  	strcat(buff, "\n");
					   //  	// cout << file.path().filename().string() << endl;
					   //  }
					   //  filelist.clear();
					   //  send(sockets[i], buff, buff_len, MSG_NOSIGNAL);
        //         	}
        //             if(strcmp(buff, "lsss") == 0) {
        //             	// cout << root << endl;
					   //  for (const auto &n: filesystem::directory_iterator{root}) filelist.push_back(n.path().filename().string());
					   //  sort(filelist.begin(), filelist.end());
					   //  for (const string &name: filelist) {
					   //  	strcat(buff, name.c_str());
					   //  	strcat(buff, "\n");
					   //  	// cout << file.path().filename().string() << endl;
					   //  }
					   //  filelist.clear();
					   //  send(sockets[i], buff, buff_len, MSG_NOSIGNAL);
        //             }
        //             else if(strcmp(buff, "putt") == 0) {
        //             	if(recv(sockets[i], buff, buff_len, MSG_WAITALL) <= 0) {
		      //               close(sockets[i]);
		      //               sockets[i] = 0;
		      //               username[i] = "";
		      //           }
		      //           else {
		      //           	filename[i] = root/(string)buff;
		      //           	file.open(filename[i], ios::out|ios::binary);
		      //           	file.close();
		      //           	if(recv(sockets[i], buff, buff_len, MSG_WAITALL) <= 0) {
			     //                close(sockets[i]);
			     //                sockets[i] = 0;
			     //                username[i] = "";
			     //            }
			     //            else filesize[i] = atoll(buff);
		      //           }
        //             }
        //             else if(strcmp(buff, "puti") == 0) {
        //             	if(recv(sockets[i], buff, buff_len, MSG_WAITALL) <= 0) {
		      //               close(sockets[i]);
		      //               sockets[i] = 0;
		      //               username[i] = "";
		      //           }
		      //           else {
		      //           	if(filesize[i] >= buff_len) {
		      //           		file.open(filename[i], ios::out|ios::binary|ios::app);
			     //            	file.write(buff, buff_len);
			     //            	file.close();
			     //            	filesize[i] -= buff_len;
		      //           	}
		      //           	else {
		      //           		file.open(filename[i], ios::out|ios::binary|ios::app);
			     //            	file.write(buff, filesize[i]);
			     //            	file.close();
		      //           	}
		      //           }
        //             }
        //             else if(strcmp(buff, "gett") == 0) {
        //             	base[i] = 0;
        //             	if(recv(sockets[i], buff, buff_len, MSG_WAITALL) <= 0) {
		      //               close(sockets[i]);
		      //               sockets[i] = 0;
		      //               username[i] = "";
		      //           }
		      //           else {
		      //           	filename[i] = root/(string)buff;
		      //           	file.open(filename[i], ios::in|ios::binary);
		      //           	if(file.is_open()) {
		      //           		send(sockets[i], "The file exists.", buff_len, MSG_NOSIGNAL);
		      //           		begin = file.tellg();
								// file.seekg(0, ios::end);
								// end = file.tellg();
		      //           		send(sockets[i], to_string(end - begin).c_str(), buff_len, MSG_NOSIGNAL);
		      //           		file.close();
		      //           	}
			     //            else send(sockets[i], "The file doesn't exists.", buff_len, MSG_NOSIGNAL);
		      //           }
        //             }
        //             else if(strcmp(buff, "geti") == 0) {
        //             	memset(buff, '\0', buff_len);
	       //          	file.open(filename[i], ios::in|ios::binary);
        //         		file.seekg(base[i], ios::beg);
        //         		file.read(buff, buff_len);
					   //  base[i] = file.tellg();
					   //  file.close();
					   //  // send(sockets[i], to_string(j).c_str(), buff_len, MSG_NOSIGNAL);
	       //          	// cout << buff << endl;
	       //          	send(sockets[i], buff, buff_len, MSG_NOSIGNAL);
	       //          	// if(file.peek() == EOF) file.close();
        //             }
        //             else if(strcmp(buff, "logi") == 0) {
        //             	memset(buff, '\0', buff_len);
        //             	if(recv(sockets[i], buff, buff_len, MSG_WAITALL) <= 0) {
		      //               close(sockets[i]);
		      //               sockets[i] = 0;
		      //               username[i] = "";
		      //           }
        //             	for(int k = 0; k < max_number_of_users; k++) {
        //             		if(strcmp(buff, username[k].c_str()) == 0) {
        //             			send(sockets[i], "username is in used, please try another:", buff_len, MSG_NOSIGNAL);
        //             			flag = false;
        //             			break;
        //             		}
        //             	}
        //             	if(!flag) continue;
        //             	send(sockets[i], "connect successfully", buff_len, MSG_NOSIGNAL);
        //             	username[i] = buff;
        //             }
                    // else cout << "Forbidden area: " << buff << endl;
                }
            }
        }
    }
	close(socket_fd);
	return 0;
}
