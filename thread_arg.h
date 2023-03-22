#ifndef THREAD_ARG_H
#define THREAD_ARG_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <queue>

#define MSG_MAX_SIZE 64*1024

enum {
    CONTINUE_THREAD,
    SEND_DATA,
    STOP_THREAD
};

struct SndThreadArg
{
    int threadNum;
    int *sockfd;
    struct sockaddr_in *addr;
    std::queue<char *> *msgQueue;
    std::queue<int> *numOfBytesQueue;
    int flag;

public:
    SndThreadArg()
        : threadNum(0),
          sockfd(new int),
          addr(new struct sockaddr_in()),
          msgQueue(new std::queue<char *>()),
          numOfBytesQueue(new std::queue<int>()),
          flag(CONTINUE_THREAD) { memset(addr, 0, sizeof(*addr)); }
    ~SndThreadArg() { close(*sockfd); delete sockfd; delete addr, delete msgQueue; }
};

struct RcvThreadArg
{
    int maxDatagramInSec;
    int *sockfd;
    int numOfServers;
    struct sockaddr_in *addr;
    SndThreadArg *sndThreadArgArr;
    int flag;

public:
    RcvThreadArg()
        : maxDatagramInSec(0),
          sockfd(new int),
          numOfServers(0),
          addr(new struct sockaddr_in()),
          sndThreadArgArr(nullptr),
          flag(CONTINUE_THREAD) { memset(addr, 0, sizeof(*addr)); }
    ~RcvThreadArg() { close(*sockfd); delete sockfd; delete addr;}
};

#endif // THREAD_ARG_H
