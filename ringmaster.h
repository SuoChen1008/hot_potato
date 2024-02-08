#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "socket.h"

class ringMaster{
  private:
    const char * port_num;
    int num_players;
    int num_hops;

  public:
    int ringmaster_socket_fd;
    std::vector<int> fd_player;
    std::vector<std::string> ip_player;
    std::vector<int> port_player;

    void setPlayernum( int n){
      num_players = n;
    }

    void setPort( const char * p){
      port_num = p;
    }

    void setHop( int m){
      num_hops = m;
    }

    int getPlayernum(){
      return num_players;
    }

    const char * getPort(){
      return port_num;
    }

    int getHop(){
      return num_hops;
    }

    void startPrint();
    void readyPrint(int i);
    void gameStart();
    void gameClose();
};