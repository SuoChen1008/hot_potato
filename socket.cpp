#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include "socket.h"

int serverConstruct(const char * port)
{
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: server cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    } 
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    } 
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    } 
    status = listen(socket_fd, 100);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl; 
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
    } 
    freeaddrinfo(host_info_list);
    return socket_fd;
}

int clientConstruct(const char * hostname, const char * port)
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: client cannot get address info for host" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    EXIT_FAILURE;
  } 
  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error: client cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    EXIT_FAILURE;
  } 

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error:client cannot connect to socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    EXIT_FAILURE;
  } 
  freeaddrinfo(host_info_list);
  return socket_fd;
}

void mySend(int socket_fd, const void *msg, int len){
  if(send(socket_fd, msg, len, 0) != len){
    std::cerr<<"Message sent is incomplete!"<<std::endl;  
  }
}

void myRecv(int socket_fd, void *buffer, int len){
  int status = recv(socket_fd, buffer, len, 0);
  if(status != 0 && status != len){
    std::cerr<<"Message received is incomplete!"<<std::endl;
  }
}

std::pair<int, int> playermasterConnect(const char *hostname, const char *port, int * id_player_addr, int  * n_players_addr) {
  int ringmaster_socket_fd =clientConstruct(hostname,port);
  myRecv(ringmaster_socket_fd, id_player_addr, sizeof(*id_player_addr));
  myRecv(ringmaster_socket_fd, n_players_addr, sizeof(*n_players_addr));
  int player_fd = serverConstruct("");
  struct sockaddr_in s;
  socklen_t size = sizeof(s);
  getsockname(player_fd, (struct sockaddr *)&s, &size);
  int player_port = ntohs(s.sin_port);
  mySend(ringmaster_socket_fd, &player_port, sizeof(player_port));
  std::cout << "Connected as player " << *id_player_addr << " out of " << *n_players_addr << " total players"<<std::endl;
  return  std::make_pair(ringmaster_socket_fd,player_fd);
}

