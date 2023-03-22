#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MSG_MAX_SIZE 64*1024

using std::cout;
using std::endl;

int main (int argc, char *argv[])
{
    if (argc != 2) {
        cout << "USAGE: " <<  argv[0] << " <number of port for receiving>" << endl;
        exit(0);
    }
    const char * fileName = argv[1];

    const char *ip = "127.0.0.1";
    int port = atoi(fileName);

    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    int n;
    n = bind(sockfd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (n < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    char buffer[MSG_MAX_SIZE];
    bzero(buffer, MSG_MAX_SIZE);
    struct sockaddr_in client_addr = {0};
    socklen_t addr_size = sizeof(client_addr);
    while (true) 
    {
        int numOfRcvBytes = recvfrom(sockfd, buffer, MSG_MAX_SIZE, 0, 
                                reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
        cout << "Data received to server: " << buffer << endl;
    }
    
    // bzero(buffer, MSG_MAX_SIZE);
    // strcpy(buffer, "Answer from server!");
    // sendto(sockfd, buffer, 1024, 0, 
    //        reinterpret_cast<struct sockaddr *>(&client_addr), addr_size);
    // cout << "Data sent from server: " << buffer << endl;

    close(sockfd);

    return 0;
}
