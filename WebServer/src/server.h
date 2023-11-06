#ifndef server_h
#define server_h

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
int createServer();

int bindServer(int Server,struct sockaddr_in *host_addr);

void file(int client, const std::string& filename);

#endif 