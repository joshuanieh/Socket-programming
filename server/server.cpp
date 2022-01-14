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
#define buff_len 4096
#define max_number_of_users 10
#define max_number_of_users_in_database INT16_MAX
using namespace std;

int main(int argc, char const *argv[]) {
	string httpRequest, fileroot = "./public", name;
	const filesystem::path root{fileroot};
	vector<string> filelist, allUsername;
	string chattingFriend[max_number_of_users_in_database], filename[max_number_of_users_in_database];
	int filesize[max_number_of_users_in_database], headerLength;
	for (const auto &n : filesystem::directory_iterator{root}) {
		if (n.is_directory())
            allUsername.push_back(n.path().stem().string());
	}
	streampos begin, end;
	streampos base[max_number_of_users_in_database], chatBase[max_number_of_users_in_database];
	bool flag, fileFlag = false;
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
            		if(recv(sockets[i], buff, headerLength + 3000, MSG_WAITALL) <= 0) {
	                    close(sockets[i]);
	                    sockets[i] = 0;
	                }
	                else {
	                	cout << sockets[i] << endl;
	                	
	                	flag = false;
	                	httpRequest = buff;
	                	// print out http request
	                	// for (int i = 0; i < httpRequest.size(); ++i) {
	                	// 	if(httpRequest[i] == '\r') cout << "\\r";
	                	// 	else if(httpRequest[i] == '\n') cout << "\\n";
	                	// 	else cout << httpRequest[i];
	                	// }
	                	cout << "httpRequest: " << httpRequest << endl;

	            		// contentLengthPos = httpRequest.find("Content-Length: ");
	            		// contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
						data = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);

	                	//Format: "FileImme{number} {data}"
						//Return: "0"
						if(data.substr(0, 8) == "FileImme") {
							// cout << "should be here" << endl;
							index = stoi(data.substr(8, data.find(" ") - 8));
							int sep = data.find(" ");
							data = data.substr(sep + 1);
							cout << "data: " << data << endl;
							// cout << "Length of data: " << data.length() << endl;
							contentLengthPos = httpRequest.find("Content-Length: ");//For binary file
	                		contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
							// cout << "contentLength: " << contentLength << endl;
							// cout << "data.find: " << data.find(" ") << endl;

							file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength - sep - 1);
							file.close();

							file.open(root/chattingFriend[index]/allUsername[index]/filename[index], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength - sep - 1);
							file.close();
							
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							strcat(httpResponse, "0");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "FileFinish{number} {file size} {data}\0\0\0\0"
						//Return: "0"
						else if(data.substr(0, 8) == "FileFinish") {
							fileFlag = false;
							int sep1 = data.find(" ");
							int sep2 = data.find(" ", sep1 + 1);
							index = stoi(data.substr(8, sep1 - 8));
							int length = stoi(data.substr(sep1 + 1, sep2 - sep1 - 1));
							data = data.substr(sep2 + 1);
							cout << "data: " << data << endl;
							
							contentLengthPos = httpRequest.find("Content-Length: ");//For binary file
	                		contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
							
							file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength - sep2 - 1);
							file.close();

							file.open(root/chattingFriend[index]/allUsername[index]/filename[index], ios::out|ios::binary|ios::app);
							file.write(data.c_str(), contentLength - sep2 - 1);
							file.close();

							file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::out|ios::app);
							file.write(("FA: " + filename[index] + "\n").c_str(), filename[index].size() + 8);
							file.close();
							chatBase[index] += filename[index].size() + 8;

							file.open((root/chattingFriend[index]/allUsername[index]).string() + ".txt", ios::out|ios::app);
							file.write(("FB: " + filename[index] + "\n").c_str(), filename[index].size() + 8);
							file.close();
							
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							strcat(httpResponse, "0");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;

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
	                }
            	}
                else {
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
	                	cout << "httpRequest: " << httpRequest << endl;

	            		// contentLengthPos = httpRequest.find("Content-Length: ");
	            		// contentLength = stoi(httpRequest.substr(contentLengthPos + 16, httpRequest.find("\r\n", contentLengthPos + 16) - contentLengthPos - 16));
						data = httpRequest.substr(httpRequest.find("\r\n\r\n") + 4);
						
	            		//Format: "Login {username}"
	            		//Return: user index
						cout << "data.substr(0, 8): " << data.substr(0, 8) << endl;
						cout << "data: " << data << endl;
						if(data.substr(0, 5) == "Login") {
							name = data.substr(6);
							int k;
							cout << name << endl;
							for(k = 0; k < allUsername.size(); k++) {
								cout << "1" << endl;
								if(allUsername[k] == name) {
									cout << "2" << endl;
					    			flag = true;
								    strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
					    			sprintf(httpResponse, "%s%d", httpResponse, k);
									cout << httpResponse << endl;
								    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
									close(sockets[i]);
									sockets[i] = 0;
					    			break;
								}
							}
							if(flag) continue;
							cout << "3" << endl;
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
			    			sprintf(httpResponse, "%s%d", httpResponse, k);
						    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
							cout << httpResponse << endl;
			    			allUsername.push_back(name);
							filesystem::create_directory(root/name);
						}

						//Format: "List friends {number}"
	            		//Return: all friends' names
	        			else if(data.substr(0, 12) == "List friends") {
	        				index = stoi(data.substr(13));

	    					for (const auto &n : filesystem::directory_iterator{root/allUsername[index]}) {
								if(is_directory(n))
									filelist.push_back(n.path().stem().string());
							}
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
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "Add {username} {number}"
						//Return: "0" | "1"
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

						    		strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
						    		strcat(httpResponse, "0");
								    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
									cout << httpResponse << endl;
									close(sockets[i]);
									sockets[i] = 0;
					    			flag = true;
					    			break;
								}
							}
							if(flag) continue;
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
				    		strcat(httpResponse, "1");
						    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							cout << httpResponse << endl;
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "Remove {username} {number}"
						//Return: "0"
						else if(data.substr(0, 6) == "Remove") {
							int sep = data.find(" ", 7);
							name = data.substr(7, sep - 7);
	        				index = stoi(data.substr(sep + 1));

							remove(((root/allUsername[index]/name).string() + ".txt").c_str());
							filesystem::remove_all(root/allUsername[index]/name);

							remove(((root/name/allUsername[index]).string() + ".txt").c_str());
							filesystem::remove_all(root/name/allUsername[index]);

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
				    		strcat(httpResponse, "0");
						    send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							cout << httpResponse << endl;
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "Chat {username} {number}"
						//Return: Chat history
						else if(data.substr(0, 4) == "Chat") {
							int sep = data.find(" ", 5);
							name = data.substr(5, sep - 5);
	        				index = stoi(data.substr(sep + 1));
	        				chattingFriend[index] = name;

							file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::in);
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n0\n");
							file.seekg (0, ios::end);
						    int length = file.tellg();
						    cout << "len: " << length << endl;
						    chatBase[index] = (buff_len - strlen(httpResponse)) > length ? length : (buff_len - strlen(httpResponse));
							file.seekg(- chatBase[index], ios::end);
							cout << chatBase[index] << endl;
							cout << file.tellg() << endl;
							memset(buff, '\0', buff_len);
							file.read(buff, chatBase[index]);
							file.close();
							strcat(httpResponse, buff);
							cout << "buff: " << buff << endl;
							cout << "response: " << httpResponse << endl;
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "More {number}"
						//Return: More chat history
						// else if(data.substr(0, 4) == "More") {
	     //    				index = stoi(data.substr(5));
						// 	file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::in);
						// 	strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n0\n");
						// 	chatBase[index] += buff_len - strlen(httpResponse);
						// 	file.seekg(chatBase[index], ios::end);
						// 	file.read(buff, buff_len - strlen(httpResponse));
						// 	file.close();
						// 	strcat(httpResponse, buff);
					 //    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
						// 	close(sockets[i]);
						// 	sockets[i] = 0;
						// }

						//Format: "Text{number} {plain text}"
						//Return: "0"
						else if(data.substr(0, 4) == "Text") {
							index = stoi(data.substr(4, data.find("") - 4));
							data = data.substr(data.find(" ") + 1);

							file.open((root/allUsername[index]/chattingFriend[index]).string() + ".txt", ios::out|ios::app);
							file.write(("A: " + data + "\n").c_str(), data.size() + 4);
							file.close();
							chatBase[index] += data.size() + 4;

							file.open((root/chattingFriend[index]/allUsername[index]).string() + ".txt", ios::out|ios::app);
							file.write(("B: " + data + "\n").c_str(), data.size() + 4);
							file.close();

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							strcat(httpResponse, "0");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;

					  //   	for(int m = 0; m < max_number_of_users; m++) {
					  //   		if (username[m] == chattingFriend[i]) {
					  //   			if(chattingFriend[m] == username[i]) {
					  //   				send(sockets[m], data.c_str(), data.size(), MSG_NOSIGNAL);
					  //   			}
					  //   			break;
					  //   		}
					  //   	}
						}

						//Format: "FileName{number} {file name}" (data size = 3000)
						//Return: "0"
						else if(data.substr(0, 8) == "FileName") {	
							fileFlag = true;
							headerLength = httpRequest.size() - data.size();

							index = stoi(data.substr(8, data.find(" ") - 8));
							filename[index] = data.substr(data.find(" ") + 1);

							file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::out|ios::binary);
							file.close();

							file.open(root/chattingFriend[index]/allUsername[index]/filename[index], ios::out|ios::binary);
							file.close();
							
							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							strcat(httpResponse, "0");
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
						}

						//Format: "Download{number} {filename}"
						//Return: "filesize" (of no need)
						else if(data.substr(0, 8) == "Download") {
							index = stoi(data.substr(8, data.find(" ") - 8));
							filename[index] = data.substr(data.find(" ") + 1);
							base[index] = 0;
							
							file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::in|ios::binary);
							file.seekg(0, ios::end);
							file.close();
							filesize[index] = file.tellg();

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							strcat(httpResponse, to_string(filesize[index]).c_str());
					    	send(sockets[i], httpResponse, strlen(httpResponse), MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
						}

						//client should detect the last download
						//Format: "DownloadImme{number}"
						//Return: File content
						else if(data.substr(0, 12) == "DownloadImme") {
							index = stoi(data.substr(12));

							strcpy(httpResponse, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
							headerLength = strlen(httpResponse);
							file.open(root/allUsername[index]/chattingFriend[index]/filename[index], ios::in|ios::binary);
							file.seekg(base[index], ios::end);
							memset(buff, '\0', buff_len);
							int length = filesize[index] > base[index] + buff_len - strlen(httpResponse) ? buff_len - strlen(httpResponse) : filesize[index] - base[index];
							file.read(buff, length);
							file.close();
							base[index] += length;

							strcat(httpResponse, buff);
					    	send(sockets[i], httpResponse, headerLength + length, MSG_NOSIGNAL);
							close(sockets[i]);
							sockets[i] = 0;
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
