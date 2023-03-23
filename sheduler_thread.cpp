#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <cmath>
#include "shedular_thread.h"

void* ReceivingThread(void* arg)
{
    RcvThreadArg *threadArg = reinterpret_cast<RcvThreadArg *>(arg);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("receiving socket error");
        pthread_exit(reinterpret_cast<void *>(EXIT_FAILURE));
    }

    int res;
    res = bind(sockfd,
               reinterpret_cast<struct sockaddr *>(threadArg->addr),
               sizeof(*threadArg->addr));
    if (res < 0) {
        perror("bind error");
        pthread_exit(reinterpret_cast<void *>(EXIT_FAILURE));
    }

    pthread_mutex_lock(threadArg->mtx);
    cout << "Started receiving thread" << endl;
    pthread_mutex_unlock(threadArg->mtx);

    char buffer[MSG_MAX_SIZE];
    int numOfRcvBytes;

    bool isProceed = true;
    while(isProceed)
    {
        switch(threadArg->flag)
        {
        case CONTINUE_THREAD:
        {
            clock_t begin = clock();

            for (int i = 0; i < threadArg->maxDatagramInSec && isProceed; i++)
            {
                memset(buffer, 0, MSG_MAX_SIZE);
                numOfRcvBytes = recv(sockfd, buffer, MSG_MAX_SIZE, 0);

                pthread_mutex_lock(threadArg->mtx);
                cout << "Data received to server: " << buffer << ", "
                     << numOfRcvBytes << " bytes" << endl;
                pthread_mutex_unlock(threadArg->mtx);

                char *sendingMsg = new char[numOfRcvBytes];
                memmove(sendingMsg, buffer, numOfRcvBytes);

                pthread_mutex_lock(threadArg->mtx);
                SndThreadArg *sndThreadArg = threadArg->sndThreadArg;
                sndThreadArg->numOfBytesQueue->push(numOfRcvBytes);
                sndThreadArg->msgQueue->push(sendingMsg);
                sndThreadArg->flag = SEND_DATA;
                pthread_mutex_unlock(threadArg->mtx);
            }

            clock_t end = clock();
            double timeSpent = static_cast<double>(end-begin) / CLOCKS_PER_SEC;

            // Пропускаем сообщения, если достигнули максимума
            while (timeSpent < 1)
            {
                memset(buffer, 0, MSG_MAX_SIZE);
                numOfRcvBytes = recv(sockfd, buffer, MSG_MAX_SIZE, 0);

                end = clock();
                timeSpent = static_cast<double>(end-begin) / CLOCKS_PER_SEC;

                if (timeSpent < 1) {
                    pthread_mutex_lock(threadArg->mtx);
                    cout << "Msg skipped..." << endl;
                    pthread_mutex_unlock(threadArg->mtx);
                    continue;
                }
                else {
                    pthread_mutex_lock(threadArg->mtx);
                    cout << "Data received to server: " << buffer << ", "
                         << numOfRcvBytes << " bytes" << endl;
                    pthread_mutex_unlock(threadArg->mtx);

                    char *sendingMsg = new char[numOfRcvBytes];
                    memmove(sendingMsg, buffer, numOfRcvBytes);

                    pthread_mutex_lock(threadArg->mtx);
                    SndThreadArg *sndThreadArg = threadArg->sndThreadArg;
                    sndThreadArg->numOfBytesQueue->push(numOfRcvBytes);
                    sndThreadArg->msgQueue->push(sendingMsg);
                    sndThreadArg->flag = SEND_DATA;
                    pthread_mutex_unlock(threadArg->mtx);

                    break;
                }
            }
            break;
        }
        case STOP_THREAD:
            isProceed = false;
            break;
        default:
            break;
        }
    }

    close(sockfd);
    pthread_mutex_lock(threadArg->mtx);
    cout << "Stoped receiving thread" << endl;
    pthread_mutex_unlock(threadArg->mtx);

    pthread_exit(0);
}

void* SendingThread(void* arg)
{
    SndThreadArg *threadArg = reinterpret_cast<SndThreadArg *>(arg);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("sending socket error");
        pthread_exit(reinterpret_cast<void *>(EXIT_FAILURE));
    }

    pthread_mutex_lock(threadArg->mtx);
    cout << "Started sending thread" << endl;
    pthread_mutex_unlock(threadArg->mtx);

    int nextAddrId = 0;
    bool isProceed = true;
    while(isProceed)
    {
        pthread_mutex_lock(threadArg->mtx);
        switch(threadArg->flag)
        {
        case CONTINUE_THREAD:
            break;
        case SEND_DATA:
        {
            char *msg = threadArg->msgQueue->front();
            int numOfSendingBytes = threadArg->numOfBytesQueue->front();
            threadArg->msgQueue->pop();
            threadArg->numOfBytesQueue->pop();

            const char * nextIp = (*threadArg->ipVec)[nextAddrId].c_str();
            int nextPort = (*threadArg->sndPortsVec)[nextAddrId];
            threadArg->addr->sin_addr.s_addr = inet_addr(nextIp);
            threadArg->addr->sin_port = htons(nextPort);

            int res = sendto(sockfd, msg,
                             numOfSendingBytes, 0,
                             reinterpret_cast<struct sockaddr *>(threadArg->addr),
                             sizeof(*threadArg->addr));
            cout << "Data sent to " << nextAddrId + 1 << " server" << endl;

            delete msg;
            nextAddrId = (nextAddrId + 1) % threadArg->ipVec->size();
            threadArg->flag = CONTINUE_THREAD;

            if (res == -1) {
                perror("sending data failed");
                exit(EXIT_FAILURE);
                break;
            }

            break;
        }
        case STOP_THREAD:
            isProceed = false;
            break;
        default:
            break;
        }
        pthread_mutex_unlock(threadArg->mtx);

        usleep(round((1/threadArg->maxDatagramInSec)*1000000));
    }

    close(sockfd);
    pthread_mutex_lock(threadArg->mtx);
    cout << "Stoped sending thread" << endl;
    pthread_mutex_unlock(threadArg->mtx);

    pthread_exit(0);
}
