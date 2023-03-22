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
    int numOfServers;
    int numOfDatagramsInSec;
    RcvThreadArg rcvThreadArg;
    SndThreadArg *sndThreadArgArr;
    pthread_t rcvThreadId;
    pthread_t *sndThreadIdArr;

public:
    NetworkSheduler(const char *configFile);
    ~NetworkSheduler();
    void start();
    static bool readConfig(const char *configFile, int &numOfServers,
                           std::list<std::string> &iplist,
                           std::list<int> &sndPortsList,
                           int &rcvPort, int &numOfDatagramsInSec);
};

#endif // NETWORK_SHEDULER_H
