en); l++) {
							sprintf(cnd, "geti");
							send(client_fd, cnd, 5, MSG_NOSIGNAL);
							recv(client_fd, buff, buff_len, MSG_WAITALL);
							file.open(root/filename, ios::app|ios::out|ios::binary);
							file.write(buff, buff_len);
							file.close();
						}
						sprintf(cnd, "geti");
						send(client_fd, cnd, 5, MSG_NOSIGNAL);
						recv(client_fd, buff, buff_len, MSG_WAITALL);
						file.open(root/filename, ios::app|ios::out|ios::binary);
						file.write(buff, filesize%buff_len);
						file.close();
						cout << "get " << filename << " successfully" << endl;
					}
					else {
						cout << "Command not found" << endl;
					}
				}
			}
		}
	}
	close(client_fd);
	return 0;
}
en); l++) {
							sprintf(cnd, "geti");
							send(client_fd, cnd, 5, MSG_NOSIGNAL);
							recv(client_fd, buff, buff_len, MSG_WAITALL);
							file.open(root/filename, ios::app|ios::out|ios::binary);
							file.write(buff, buff_len);
							file.close();
						}
						sprintf(cnd, "geti");
						send(client_fd, cnd, 5, MSG_NOSIGNAL);
						recv(client_fd, buff, buff_len, MSG_WAITALL);
						file.open(root/filename, ios::app|ios::out|ios::binary);
						file.write(buff, filesize%buff_len);
						file.close();
						cout << "get " << filename << " successfully" << endl;
					}
					else {
						cout << "Command not found" << endl;
					}
				}
			}
		}
	}
	close(client_fd);
	return 0;
}
en); l++) {
							sprintf(cnd, "geti");
							send(client_fd, cnd, 5, MSG_NOSIGNAL);
							recv(client_fd, buff, buff_len, MSG_WAITALL);
							file.open(root/filename, ios::app|ios::out|ios::binary);
							file.write(buff, buff_len);
							file.close();
						}
						sprintf(cnd, "geti");
						send(client_fd, cnd, 5, MSG_NOSIGNAL);
						recv(client_fd, buff, buff_len, MSG_WAITALL);
						file.open(root/filename, ios::app|ios::out|ios::binary);
						file.write(buff, filesize%buff_len);
						file.close();
						cout << "get " << filename << " successfully" << endl;
					}
					else {
						cout << "Command not found" << endl;
					}
				}
			}
		}
	}
	close(client_fd);
	return 0;
}
all: server
	@echo -n ""

server: server.o
	g++ $^ -o $@
server.o: server.cpp
	g++ -std=c++17 -c $^ -o $@

clean:
	rm -rf server server.o client_dir/

clean_all:
	rm -rf server server.o client_dir/ public/*
