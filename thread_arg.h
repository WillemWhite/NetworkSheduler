#ifndef THREAD_ARG_H
#define THREAD_ARG_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <queue>
#include <list>

#define MSG_MAX_SIZE 64*1024

enum {
    CONTINUE_THREAD,
    SEND_DATA,
    STOP_THREAD
};

struct SndThreadArg
{
    pthread_mutex_t *mtx;
    int maxDatagramInSec;
    struct sockaddr_in *addr;
    std::vector<std::string> *ipVec;
    std::vector<int> *sndPortsVec;
    std::queue<char *> *msgQueue;
    std::queue<int> *numOfBytesQueue;
    int flag;

public:
    SndThreadArg()
        : mtx(nullptr),
          maxDatagramInSec(0),
          addr(new struct sockaddr_in()),
          ipVec(nullptr),
          sndPortsVec(nullptr),
          msgQueue(new std::queue<char *>()),
          numOfBytesQueue(new std::queue<int>()),
          flag(CONTINUE_THREAD) { memset(addr, 0, sizeof(*addr)); }
    ~SndThreadArg() { delete addr, delete msgQueue; }
};

struct RcvThreadArg
{
    pthread_mutex_t *mtx;
    int maxDatagramInSec;
    struct sockaddr_in *addr;
    SndThreadArg *sndThreadArg;
    int flag;

public:
    RcvThreadArg()
        : mtx(nullptr),
          maxDatagramInSec(0),
          addr(new struct sockaddr_in()),
          sndThreadArg(nullptr),
          flag(CONTINUE_THREAD) { memset(addr, 0, sizeof(*addr)); }
    ~RcvThreadArg() { delete addr;}
};

#endif // THREAD_ARG_H
