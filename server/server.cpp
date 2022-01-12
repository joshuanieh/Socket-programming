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
#include "sha1.cpp"

#define socket_domain AF_INET
#define socket_type SOCK_STREAM
#define socket_protocol 0
#define buff_len 1024
#define max_number_of_users 10
using namespace std;

int main(int argc, char const *argv[]) {
	int webSocketKeyPos;
	string webSocketAccept, webSocketKey;
	string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
	
	vector<string> filelist, allUsername;
	streampos begin, end, base[max_number_of_users], chatBase[max_number_of_users];
	bool flag;
	string httpRequest, fileroot = "./", name;
	const filesystem::path root{fileroot};
	int o = true, j;
	fstream file;
	long long filesize[max_number_of_users];
	char buff[buff_len], c, httpResponse[buff_len];
	string filename[max_number_of_users], username[max_number_of_users] = {""}, chattingFriend[max_number_of_users] = {""}, data;
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
                    username[i] = "";
                	chattingFriend[i] = "";
                }
                else{
					cout << sockets[i] << endl;
                	
                	flag = false;
                	httpRequest = buff;
                	// print out http request
                	for (int i = 0; i < httpRequest.size(); ++i) {
                		if(httpRequest[i] == '\r') cout << "\\r";
                		else if(httpRequest[i] == '\n') cout << "\\n";
                		else cout << httpRequest[i];
                	}
                	cout << endl;

                	if(httpRequest.substr(0, 3) == "GET") {
                		//Handshake
                		if(httpRequest.find("Sec-WebSocket-Key: ") != string::npos) {
							webSocketKeyPos = httpRequest.find("Sec-WebSocket-Key: ");
							if(webSocketKeyPos != string::npos) {
								cout << "need Accept header" << endl;
								webSocketKey = httpRequest.substr(webSocketKeyPos + 19, httpRequest.find("\r\n", webSocketKeyPos + 19) - webSocketKeyPos - 19);
								webSocketAccept = webSocketAcceptGenerate(webSocketKey);
								// cin >> webSocketAccept;
								response = response + webSocketAccept + "\r\n\r\n";
								cout << response << endl;
								strcpy(buff, response.c_str());
								send(sockets[i], buff, response.size(), MSG_NOSIGNAL);
								// cout << "send finish" << endl;
							}
						}
                	}

                	else if(httpRequest.substr(0, 4) == "POST") {
                		// contentLengthPos = httpRequest.find("Content-Length: ");
                		// contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
                		data = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
						
                		//Format: "Login {username}"
						if(data.substr(0, 5) == "Login") {
							name = data.substr(6);
			    			username[i] = name;
							for(auto &user : allUsername) {
								if(user == name) {
					    			flag = true;
					    			break;
								}
							}
							if(flag) continue;
							allUsername.push_back(name);
							filesystem::create_directory(root/username[i]);
						}

						//Format: "List friends"
                		//Return: all friends' names
            			else if(data.substr(0, 12) == "List friends") {
        					for (const auto &n : filesystem::directory_iterator{root/username[i]}) filelist.push_back(n.path().stem().string());
						    sort(filelist.begin(), filelist.end());
						    strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
						    for (const string &friendName : filelist) {
						    	strcat(httpResponse, friendName.c_str());
						    	strcat(httpResponse, "\n");
						    	// cout << file.path().filename().string() << endl;
						    }
						    filelist.clear();
						    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						}

						//Format: "Add {username}"
						//Return: "Add successfully" | "Add fail"
						else if(data.substr(0, 3) == "Add") {
							name = data.substr(4);
							for(auto &user : allUsername) {
								if(user == name) {
									file.open((root/username[i]/name).string() + ".txt", ios::out|ios::binary|ios::app);
				                	file.close();
									filesystem::create_directory(root/username[i]/name);

									file.open((root/name/username[i]).string() + ".txt", ios::out|ios::binary|ios::app);
				                	file.close();
									filesystem::create_directory(root/name/username[i]);

						    		strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
						    		strcat(httpResponse, "Add successfully");
								    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
					    			flag = true;
					    			break;
								}
							}
							if(flag) continue;
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
				    		strcat(httpResponse, "Add fail");
						    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						}

						//Format: "Remove {username}"
						else if(data.substr(0, 6) == "Remove") {
							name = data.substr(7);
							
							remove(((root/username[i]/name).string() + ".txt").c_str());
							filesystem::remove_all(root/username[i]/name);

							remove(((root/name/username[i]).string() + ".txt").c_str());
							filesystem::remove_all(root/name/username[i]);
						}

						//Format: "Chat {username}"
						else if(data.substr(0, 4) == "Chat") {
							chattingFriend[i] = data.substr(5);
							file.open((root/username[i]/chattingFriend[i]).string() + ".txt", ios::in);
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
							chatBase[i] = buff_len - strlen(httpResponse);
							file.seekg(chatBase[i], ios::end);
							file.read(buff, buff_len - strlen(httpResponse));
							file.close();
							strcat(httpResponse, buff);
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						}

						//Format: "More"
						else if(data.substr(0, 4) == "More") {
							file.open((root/username[i]/chattingFriend[i]).string() + ".txt", ios::in);
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
							chatBase[i] += buff_len - strlen(httpResponse);
							file.seekg(chatBase[i], ios::end);
							file.read(buff, buff_len - strlen(httpResponse));
							file.close();
							strcat(httpResponse, buff);
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						}

						//Format: "Text {plain text}"
						//Return: "Text finish" to sender & "{data}" to receiver
						else if(data.substr(0, 4) == "Text") {
							data = data.substr(5);

							file.open((root/username[i]/chattingFriend[i]).string() + ".txt", ios::out|ios::app);
							file.write(("A: " + data + "\n").c_str(), data.size() + 4);
							file.close();

							file.open((root/chattingFriend[i]/username[i]).string() + ".txt", ios::out|ios::app);
							file.write(("B: " + data + "\n").c_str(), data.size() + 4);
							file.close();

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
							strcat(httpResponse, "Text finish");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);

					    	for(int m = 0; m < max_number_of_users; m++) {
					    		if (username[m] == chattingFriend[i]) {
					    			if(chattingFriend[m] == username[i]) {
					    				send(sockets[m], data.c_str(), data.size(), MSG_NOSIGNAL);
					    			}
					    			break;
					    		}
					    	}
						}

						//Format: "FileName {file name}"
						else if(data.substr(0, 8) == "FileName") {
							// int pos = data.find("FileSize");
							filename[i] = data.substr(9);
                			// filesize[i] = atoll(data.substr(pos + 9));

							file.open(root/username[i]/chattingFriend[i]/filename[i], ios::out|ios::binary);
							file.close();

							file.open(root/chattingFriend[i]/username[i]/filename[i], ios::out|ios::binary);
							file.close();
						}

						//Format: "FileImme {data}"
						else if(data.substr(0, 8) == "FileImme") {
							data = data.substr(9);
							contentLengthPos = httpRequest.find("Content-Length: ");
	                		contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
							
							file.open(root/username[i]/chattingFriend[i]/filename[i], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength);
							file.close();

							file.open(root/chattingFriend[i]/username[i]/filename[i], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength);
							file.close();
						}

						//Format: "FileFinish"
						//Return: "File finish" to sender & "Filename {filename}" to receiver
						else if(data.substr(0, 8) == "FileFinish") {
							file.open((root/username[i]/chattingFriend[i]).string() + ".txt", ios::out|ios::app);
							file.write(("AFile: " + filename[i] + "\n").c_str(), filename[i].size() + 8);
							file.close();

							file.open((root/chattingFriend[i]/username[i]).string() + ".txt", ios::out|ios::app);
							file.write(("BFile: " + filename[i] + "\n").c_str(), filename[i].size() + 8);
							file.close();

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
							strcat(httpResponse, "File finish");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);

					    	for(int m = 0; m < max_number_of_users; m++) {
					    		if (username[m] == chattingFriend[i]) {
					    			if(chattingFriend[m] == username[i]) {
					    				send(sockets[m], ("Filename " + filename[i]).c_str(), ("Filename " + filename[i]).size(), MSG_NOSIGNAL);
					    			}
					    			break;
					    		}
					    	}
						}

						//Format: "Download {filename}"
						else if(data.substr(0, 8) == "Download") {
							filename[i] = data.substr(9);
							base[i] = 0;
						}

						//Format: "DownloadImme"
						else if(data.substr(0, 12) == "DownloadImme") {
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\n\r\n");
							file.open(root/username[i]/chattingFriend[i]/filename[i], ios::in|ios::binary);
							file.seekg(base[i], ios::end);
							file.read(buff, buff_len - strlen(httpResponse));
							base[i] += buff_len - strlen(httpResponse);
							strcat(httpResponse, buff);
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						}

						//Format: "LeaveChat"
						else if(data.substr(0, 9) == "LeaveChat") {
							chattingFriend[i] = "";
							chatBase[i] = 0;
							base[i] = 0;
							filename[i] = "";
						}
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
