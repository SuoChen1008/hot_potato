#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include <limits.h>
#include "potato.h"
#include "socket.h"
#include "neighbor.h"

int main(int argc, char * argv[]) {
    if (argc != 3) {
        std::cout << "player <machine_name> <port_num>" << std::endl;
        return 1;
    }
    const char *hostname = argv[1];
    const char *port = argv[2];
    int id_player = 0;
    int n_players=0;
    std::pair<int, int> fd_pair = playermasterConnect(hostname, port, &id_player, &n_players);
    int id_leftplayer = id_player==0?n_players-1:id_player-1;
    int id_rightplayer = id_player==n_players-1?0:id_player+1;
    int ringmaster_socket_fd = fd_pair.first;
    int player_fd = fd_pair.second;

    //player receives left neighbor information from ring master
    neighborData nD;
    recv(ringmaster_socket_fd, &nD, sizeof(nD), MSG_WAITALL);
    char neigh_port[100];
    sprintf(neigh_port, "%d", nD.port);
    int left_neigh_socket_fd = clientConstruct(nD.ip,neigh_port); 
    struct sockaddr_storage socket_ADDR;
    socklen_t socket_size = sizeof(socket_ADDR);
    int right_neigh_socket_fd = accept( player_fd, (struct sockaddr *)&socket_ADDR, &socket_size);
    std::vector<int> master_left_right;
    master_left_right.push_back(ringmaster_socket_fd);
    master_left_right.push_back(left_neigh_socket_fd);
    master_left_right.push_back(right_neigh_socket_fd);
    int fd_max = INT_MIN;
    for (int i = 0; i < sizeof( master_left_right); i++) {
        fd_max = fd_max > master_left_right[i]?fd_max:master_left_right[i];
    }
    int max_socket_fd=fd_max+1;

    //start the game
    myPotato potato;
    fd_set read_fds;
    srand((unsigned int)time(NULL) + id_player);
    while(true){
        FD_ZERO(&read_fds);
        FD_SET(ringmaster_socket_fd, &read_fds);
        FD_SET(left_neigh_socket_fd, &read_fds);
        FD_SET(right_neigh_socket_fd, &read_fds);
        int activity = select( max_socket_fd, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
          std::cerr << "select() error" << std::endl;
          return 1;
        } 
        if (activity > 0){
            for (int i = 0; i < sizeof( master_left_right); i++) {
                if (FD_ISSET(master_left_right[i], &read_fds)) {
                    if (recv(master_left_right[i], &potato, sizeof(potato), MSG_WAITALL)!=sizeof(potato)){
                    std::cerr<<"Message received is incomplete!"<<std::endl;
                }
                break;
                }
            }
        }
        //end the game
        if (potato.nhops == 0) {
            break;
        }
        potato.nhops--;
        potato.path[potato.count] = id_player;
        potato.count++;
        //send the potato to the ringmsater
        if (potato.nhops == 0){
            mySend(ringmaster_socket_fd,&potato, sizeof(potato));
            std::cout<<"I'm it"<<std::endl;
        }
        //send the potato to either left or right neighbor
        else{
            int rd = rand() % 2;
            if (rd==0){
                mySend(right_neigh_socket_fd, &potato, sizeof(potato));
                std::cout << "Sending potato to " << id_rightplayer << std::endl;
            }
            else{
                mySend(left_neigh_socket_fd, &potato, sizeof(potato));
                std::cout << "Sending potato to " << id_leftplayer << std::endl;
            }

        }
    }

    close(ringmaster_socket_fd);
    close(left_neigh_socket_fd);
    close(right_neigh_socket_fd);
    return 0;
}