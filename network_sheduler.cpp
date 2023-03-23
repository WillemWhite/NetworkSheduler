#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include <list>
#include "network_sheduler.h"
#include "shedular_thread.h"

NetworkSheduler::NetworkSheduler(const char *configFile)
{
    this->configFile = configFile;
    NetworkSheduler::readConfig();

    mtx = PTHREAD_MUTEX_INITIALIZER;

    sndThreadArg.mtx = &mtx;
    sndThreadArg.maxDatagramInSec = maxDatagramInSec;
    sndThreadArg.addr->sin_family = AF_INET;
    sndThreadArg.ipVec = &ipVec;
    sndThreadArg.sndPortsVec = &sndPortsVec;

    rcvThreadArg.mtx = &mtx;
    rcvThreadArg.maxDatagramInSec = maxDatagramInSec;
    rcvThreadArg.addr->sin_family = AF_INET;
    rcvThreadArg.addr->sin_addr.s_addr = inet_addr("127.0.0.1");
    rcvThreadArg.addr->sin_port = htons(rcvPort);
    rcvThreadArg.sndThreadArg = &sndThreadArg;
}

NetworkSheduler::~NetworkSheduler()
{
    pthread_mutex_destroy(&mtx);

    rcvThreadArg.flag = STOP_THREAD;
    pthread_join(rcvThreadId, 0);

    sndThreadArg.flag = STOP_THREAD;
    pthread_join(sndThreadId, 0);
}

void NetworkSheduler::start()
{
    if (int ret = pthread_create(&rcvThreadId, NULL,
                                 ReceivingThread, &rcvThreadArg) != 0) {
        cout << "pthread_create error for receiving thread: " << strerror(ret) << endl;
        exit(EXIT_FAILURE);
    }

    if(int ret = pthread_create(&sndThreadId, NULL,
                                SendingThread, &sndThreadArg) != 0) {
        cout << "pthread_create error for sending thread: " << strerror(ret) << endl;
        exit(EXIT_FAILURE);
    }
}

bool NetworkSheduler::readConfig()
{
    std::ifstream in(configFile);
    if (!in.is_open()) {
        std::cout << "Cannot open config file "
                  << configFile << " " << strerror(errno);
        return false;
    }

    std::string param;
    std::string value;
    std::list<std::string> sendingAddrList;
    while (!in.eof())
    {
        in >> param;
        in >> value;

        if (param == "RECEIVING_PORT")
            rcvPort = stoi(value);
        if (param == "SENDING_ADDRESS")
            sendingAddrList.push_back(value);
        if (param == "NUM_OF_DATAGRAMS_PER_SEC")
            maxDatagramInSec = stoi(value);
    }

    in.close();

    numOfServers = sendingAddrList.size();
    for (int i = 0; i < numOfServers; i++)
    {
        std::string addr = sendingAddrList.front();
        sendingAddrList.pop_front();
        int sepIndex;
        sepIndex = addr.find(":");
        ipVec.push_back(addr.substr(0, sepIndex));
        addr.replace(0, sepIndex + 1, "");
        sndPortsVec.push_back(stoi(addr));
    }

    return true;
}

