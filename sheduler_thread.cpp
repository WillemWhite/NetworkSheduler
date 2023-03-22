#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <ctime>
#include <cmath>
#include "shedular_thread.h"

void* ReceivingThread(void* arg)
{
    RcvThreadArg *threadArg = reinterpret_cast<RcvThreadArg *>(arg);
    cout << "Started receiving thread\n";
    char buffer[MSG_MAX_SIZE];

    bool isProceed = true;
    while(isProceed)
    {
        switch(threadArg->flag)
        {
        case CONTINUE_THREAD:
        {
            double timeSpent = 0.0;
            clock_t begin = clock();

            for (int i = 0; i < threadArg->maxDatagramInSec && isProceed; i++)
            {
                memset(buffer, 0, MSG_MAX_SIZE);
                int numOfRcvBytes = recv(*threadArg->sockfd, buffer, MSG_MAX_SIZE, 0);
                cout << "Data received to server: " << buffer << ", "
                     << numOfRcvBytes << " bytes" << endl;

                // Тут реализовал случайный выбор следующего сервера по
                // относительно равномерному распределению. Можно и, конечно,
                // выбирать сервер с наименьшей нагрузкой, но решил поэкспериментировать.
                int sendingThreadIndex = rand() % threadArg->numOfServers;
                char *sendingMsg = new char[numOfRcvBytes];
                memmove(sendingMsg, buffer, numOfRcvBytes);
                SndThreadArg *sndThreadArg = &threadArg->sndThreadArgArr[sendingThreadIndex];
                sndThreadArg->numOfBytesQueue->push(numOfRcvBytes);
                sndThreadArg->msgQueue->push(sendingMsg);
                sndThreadArg->flag = SEND_DATA;
            }

            clock_t end = clock();
            timeSpent += static_cast<double>(end-begin) / CLOCKS_PER_SEC;
            if (timeSpent > 1)
                break;
            else {
                int timeToSleep = round((1.0 - timeSpent) * 1000000);
                usleep(timeToSleep);
                break;
            }
        }
        case STOP_THREAD:
            isProceed = false;
            break;
        default:
            break;
        }
    }

    cout << "Stoped receiving thread\n";
    pthread_exit(0);
}

void* SendingThread(void* arg)
{
    SndThreadArg *threadArg = reinterpret_cast<SndThreadArg *>(arg);
    std::string outputMsg = "Started ";
    outputMsg += std::to_string(threadArg->threadNum);
    outputMsg += " sending thread\n";
    cout << outputMsg;

    bool isProceed = true;
    while(isProceed)
    {
        switch(threadArg->flag)
        {
        case CONTINUE_THREAD:
            break;
        case SEND_DATA:
        {
            char *msg = threadArg->msgQueue->front();
            int numOfSendingBytes = threadArg->numOfBytesQueue->front();
            int res = sendto(*threadArg->sockfd, msg,
                             numOfSendingBytes, 0,
                             reinterpret_cast<struct sockaddr *>(threadArg->addr),
                             sizeof(*threadArg->addr));
            threadArg->msgQueue->pop();
            threadArg->numOfBytesQueue->pop();

            if (res == -1) {
                perror("sending data failed");
                exit(EXIT_FAILURE);
                break;
            }

            delete msg;
            threadArg->flag = CONTINUE_THREAD;
            break;
        }
        case STOP_THREAD:
            isProceed = false;
            break;
        default:
            break;
        }
    }

    outputMsg = "Stoped ";
    outputMsg += std::to_string(threadArg->threadNum);
    outputMsg += " sending thread\n";
    cout << outputMsg;
    pthread_exit(0);
}
