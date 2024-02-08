#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int serverConstruct(const char * port);
int clientConstruct(const char * hostname, const char * port);
void mySend(int socket_fd, const void *msg, int len);
void myRecv(int socket_fd, void *buffer, int len);
std::pair<int, int> playermasterConnect(const char *hostname, const char *port,int * id_player_addr, int  * n_players_addr);
