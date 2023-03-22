#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include <list>
#include "network_sheduler.h"
#include "shedular_thread.h"

NetworkSheduler::NetworkSheduler(const char *configFile)
{
    int numOfServers;
    std::list<std::string> ipList;
    std::list<int> sndPortsList;
    int rcvPort;
    int numOfDatagramsInSec;
    NetworkSheduler::readConfig(configFile, numOfServers, ipList,
                                sndPortsList, rcvPort, numOfDatagramsInSec);
    this->numOfServers = numOfServers;
    this->numOfDatagramsInSec = numOfDatagramsInSec;;

    sndThreadArgArr = new SndThreadArg[numOfServers];
    for (int i = 0; i < numOfServers; i++)
    {
        sndThreadArgArr[i].threadNum = i+1;

        *sndThreadArgArr[i].sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (*sndThreadArgArr[i].sockfd < 0) {
            perror("sending socket error");
            exit(EXIT_FAILURE);
        }

        sndThreadArgArr[i].addr->sin_family = AF_INET;
        sndThreadArgArr[i].addr->sin_port = htons(sndPortsList.front());
        sndThreadArgArr[i].addr->sin_addr.s_addr = inet_addr(ipList.front().c_str());

        sndPortsList.pop_front();
        ipList.pop_front();
    }

    rcvThreadArg.maxDatagramInSec = this->numOfDatagramsInSec;

    *rcvThreadArg.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*rcvThreadArg.sockfd < 0) {
        perror("receiving socket error");
        exit(EXIT_FAILURE);
    }

    rcvThreadArg.numOfServers = this->numOfServers;

    (*rcvThreadArg.addr).sin_family = AF_INET;
    (*rcvThreadArg.addr).sin_port = htons(rcvPort);
    (*rcvThreadArg.addr).sin_addr.s_addr = inet_addr("127.0.0.1");

    rcvThreadArg.sndThreadArgArr = this->sndThreadArgArr;

    int res;
    res = bind(*rcvThreadArg.sockfd,
               reinterpret_cast<struct sockaddr *>(rcvThreadArg.addr),
               sizeof(*rcvThreadArg.addr));
    if (res < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
}

NetworkSheduler::~NetworkSheduler()
{
    rcvThreadArg.flag = STOP_THREAD;
    pthread_join(rcvThreadId, 0);
    for (int i = 0; i < numOfServers; i++)
        sndThreadArgArr[i].flag = STOP_THREAD;
    for (int i = 0; i < numOfServers; i++)
        pthread_join(sndThreadIdArr[i], 0);

    delete [] sndThreadArgArr;
    delete [] sndThreadIdArr;
}

void NetworkSheduler::start()
{
    if (int ret = pthread_create(&rcvThreadId,
                                NULL, ReceivingThread, &rcvThreadArg) != 0) {
        cout << "pthread_create error for rcv thread: " << strerror(ret) << endl;
        exit(EXIT_FAILURE);
    }

    sndThreadIdArr = new pthread_t[numOfServers];
    for (int i = 0; i < numOfServers; i++)
    {
        if(int ret = pthread_create(&sndThreadIdArr[i], NULL,
                                    SendingThread, &sndThreadArgArr[i]) != 0) {
            cout << "pthread_create error for " << i+1 << " snd thread: "
                 << strerror(ret) << endl;
            exit(EXIT_FAILURE);
        }
    }
}

bool NetworkSheduler::readConfig(const char *configFile, int &numOfServers,
                                 std::list<std::string> &ipList,
                                 std::list<int> &sndPortsList,
                                 int &rcvPort, int &numOfDatagramsInSec)
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
            numOfDatagramsInSec = stoi(value);
    }

    in.close();

    numOfServers = sendingAddrList.size();
    for (int i = 0; i < numOfServers; i++)
    {
        std::string addr = sendingAddrList.front();
        sendingAddrList.pop_front();
        int sepIndex;
        sepIndex = addr.find(":");
        ipList.push_front(addr.substr(0, sepIndex));
        addr.replace(0, sepIndex + 1, "");
        sndPortsList.push_front(stoi(addr));
    }

    return true;
}

