#ifndef NETWORK_SHEDULER_H
#define NETWORK_SHEDULER_H

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include "thread_arg.h"
#include "shedular_thread.h"

class NetworkSheduler
{
    const char *configFile;
    int rcvPort;
    int numOfServers;
    int maxDatagramInSec;
    std::vector<std::string> ipVec;
    std::vector<int> sndPortsVec;

    pthread_mutex_t mtx;
    RcvThreadArg rcvThreadArg;
    SndThreadArg sndThreadArg;
    pthread_t rcvThreadId;
    pthread_t sndThreadId;

public:
    NetworkSheduler(const char *configFile);
    ~NetworkSheduler();
    void start();
    bool readConfig();
};

#endif // NETWORK_SHEDULER_H
