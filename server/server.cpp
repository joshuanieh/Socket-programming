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
#define buff_len 1024
#define max_number_of_users 10
using namespace std;

int main(int argc, char const *argv[]) {
	string httpRequest, fileroot = "./public", name;
	const filesystem::path root{fileroot};
	vector<string> filelist, allUsername, chattingFriend, filename;
	for (const auto &n : filesystem::directory_iterator{root}) {
		if (n.is_directory())
            allUsername.push_back(n.path().stem().string());
	}
	streampos begin, end;
	vector<streampos> base, chatBase;
	bool flag;
	int o = true, j, index;
	fstream file;
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
                if(recv(sockets[i], buff, buff_len, 0) <= 0) {
                    close(sockets[i]);
                    sockets[i] = 0;
                }
                else{
					cout << sockets[i] << endl;
                	
                	flag = false;
                	httpRequest = buff;
                	// print out http request
                	// for (int i = 0; i < httpRequest.size(); ++i) {
                	// 	if(httpRequest[i] == '\r') cout << "\\r";
                	// 	else if(httpRequest[i] == '\n') cout << "\\n";
                	// 	else cout << httpRequest[i];
                	// }
                	cout << httpRequest << endl;

            		// contentLengthPos = httpRequest.find("Content-Length: ");
            		// contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
					data = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
					
            		//Format: "Login {username}"
            		//Return: user index
					if(data.substr(0, 5) == "Login") {
						name = data.substr(6);
						int k;
						for(k; k < allUsername.size(); i++) {
							if(allUsername[k] == name) {
				    			flag = true;
							    strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
				    			sprintf(httpResponse, "%s%d", httpResponse, k);
								cout << httpResponse << endl;
							    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
				    			break;
							}
						}
						if(flag) continue;
						strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
		    			sprintf(httpResponse, "%s%d", httpResponse, k);
					    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
		    			allUsername.push_back(name);
						filesystem::create_directory(root/name);
					}

					//Format: "List friends {number}"
            		//Return: all friends' names
        			else if(data.substr(0, 12) == "List friends") {
        				index = stoi(data.substr(13));

    					for (const auto &n : filesystem::directory_iterator{root/allUsername[index]}) filelist.push_back(n.path().stem().string());
					    sort(filelist.begin(), filelist.end());
					    strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
					    for (const string &friendName : filelist) {
					    	strcat(httpResponse, friendName.c_str());
					    	strcat(httpResponse, "\n");
					    	// cout << file.path().filename().string() << endl;
					    }
					    filelist.clear();
						cout << httpResponse << endl;
					    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					}

					//Format: "Add {username} {number}"
					//(removed)Return: "Add successfully" | "Add fail"
					else if(data.substr(0, 3) == "Add") {
						int sep = data.find(" ", 4);
						name = data.substr(4, sep - 4);
        				index = stoi(data.substr(sep + 1));

						for(auto &user : allUsername) {
							if(user == name) {
								file.open((root/allUsername[index]/name).string() + ".txt", ios::out|ios::binary|ios::app);
			                	file.close();
								filesystem::create_directory(root/allUsername[index]/name);

								file.open((root/name/allUsername[index]).string() + ".txt", ios::out|ios::binary|ios::app);
			                	file.close();
								filesystem::create_directory(root/name/allUsername[index]);

					    		// strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
					    		// strcat(httpResponse, "Add successfully");
							    // send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
				    			// flag = true;
				    			break;
							}
						}
						// if(flag) continue;
						// strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
			   //  		strcat(httpResponse, "Add fail");
					 //    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					}

					//Format: "Remove {username} {number}"
					else if(data.substr(0, 6) == "Remove") {
						int sep = data.find(" ", 7);
						name = data.substr(7, sep - 7);
        				index = stoi(data.substr(sep + 1));

						remove(((root/allUsername[index]/name).string() + ".txt").c_str());
						filesystem::remove_all(root/allUsername[index]/name);

						remove(((root/name/allUsername[index]).string() + ".txt").c_str());
						filesystem::remove_all(root/name/allUsername[index]);
					}

					//Format: "Chat {username} {number}"
					else if(data.substr(0, 4) == "Chat") {
						int sep = data.find(" ", 5);
						name = data.substr(5, sep - 5);
        				index = stoi(data.substr(sep + 1));
        				chattingFriend[index] = name;

						file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::in);
						strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						chatBase[index] = buff_len - strlen(httpResponse);
						file.seekg(chatBase[index], ios::end);
						file.read(buff, buff_len - strlen(httpResponse));
						file.close();
						strcat(httpResponse, buff);
				    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					}

					//Format: "More {number}"
					else if(data.substr(0, 4) == "More") {
        				index = stoi(data.substr(5));
						file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::in);
						strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						chatBase[index] += buff_len - strlen(httpResponse);
						file.seekg(chatBase[index], ios::end);
						file.read(buff, buff_len - strlen(httpResponse));
						file.close();
						strcat(httpResponse, buff);
				    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					}

					//Format: "Text{number} {plain text}"
					else if(data.substr(0, 4) == "Text") {
						index = stoi(data.substr(4, data.find("") - 4));
						data = data.substr(data.find(" ") + 1);

						file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::out|ios::app);
						file.write(("A: " + data + "\n").c_str(), data.size() + 4);
						file.close();

						file.open((root/chattingFriend[index]/allUsername[index]).string() + ".txt", ios::out|ios::app);
						file.write(("B: " + data + "\n").c_str(), data.size() + 4);
						file.close();

						// strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						// strcat(httpResponse, "Text finish");
				  //   	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);

				  //   	for(int m = 0; m < max_number_of_users; m++) {
				  //   		if (username[m] == chattingFriend[i]) {
				  //   			if(chattingFriend[m] == username[i]) {
				  //   				send(sockets[m], data.c_str(), data.size(), MSG_NOSIGNAL);
				  //   			}
				  //   			break;
				  //   		}
				  //   	}
					}

					//Format: "FileName{number} {file name}"
					else if(data.substr(0, 8) == "FileName") {
						// int pos = data.find("FileSize");
						index = stoi(data.substr(8, data.find(" ") - 8));
						filename[index] = data.substr(data.find(" ") + 1);
            			// filesize[i] = atoll(data.substr(pos + 9));

						file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::out|ios::binary);
						file.close();

						file.open(root/chattingFriend[index]/allUsername[index]/filename[index], ios::out|ios::binary);
						file.close();
					}

					//Format: "FileImme{number} {data}"
					else if(data.substr(0, 8) == "FileImme") {
						index = stoi(data.substr(8, data.find(" ") - 8));
						data = data.substr(data.find(" ") + 1);
						contentLengthPos = httpRequest.find("Content-Length: ");//For binary file
                		contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
						
						file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::out|ios::binary|ios::app);
						file.write(data.c_str(), contentLength);
						file.close();

						file.open(root/chattingFriend[index]/allUsername[index]/filename[index], ios::out|ios::binary|ios::app);
						file.write(data.c_str(), contentLength);
						file.close();
					}

					//Format: "FileFinish {number}"
					else if(data.substr(0, 8) == "FileFinish") {
						index = stoi(data.substr(9));
						file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::out|ios::app);
						file.write(("AFile: " + filename[index] + "\n").c_str(), filename[index].size() + 8);
						file.close();

						file.open((root/chattingFriend[index]/allUsername[index]).string() + ".txt", ios::out|ios::app);
						file.write(("BFile: " + filename[index] + "\n").c_str(), filename[index].size() + 8);
						file.close();

						// strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						// strcat(httpResponse, "File finish");
				  //   	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);

				  //   	for(int m = 0; m < max_number_of_users; m++) {
				  //   		if (username[m] == chattingFriend[i]) {
				  //   			if(chattingFriend[m] == username[i]) {
				  //   				send(sockets[m], ("Filename " + filename[i]).c_str(), ("Filename " + filename[i]).size(), MSG_NOSIGNAL);
				  //   			}
				  //   			break;
				  //   		}
				  //   	}
					}

					//Format: "Download{number} {filename}"
					else if(data.substr(0, 8) == "Download") {
						index = stoi(data.substr(8, data.find(" ") - 8));
						filename[index] = data.substr(data.find(" ") + 1);
						base[index] = 0;
					}

					//client should detect the last download
					//Format: "DownloadImme{number}"
					else if(data.substr(0, 12) == "DownloadImme") {
						index = stoi(data.substr(12));

						strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::in|ios::binary);
						file.seekg(base[index], ios::end);
						file.read(buff, buff_len - strlen(httpResponse));
						base[index] += buff_len - strlen(httpResponse);
						strcat(httpResponse, buff);
				    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					}

                	// httpRequestLine = "";
                	// for (int i = 0; i < strlen(httpRequest); ++i)
                	// {
                	// 	if(httpRequest[i] == '\n') continue;
                	// 	else if (httpRequest[i] == '\r' && httpRequest[i + 1] == '\n')
                	// 	{
                	// 		cout << httpRequestLine << endl;

                	// 		httpRequestLine = "";
                	// 		continue;
                	// 	}
                	// 	httpRequestLine += httpRequest[i];
                	// }


                	


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
