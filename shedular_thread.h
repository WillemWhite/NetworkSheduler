#ifndef SHEDULAR_THREAD_H
#define SHEDULAR_THREAD_H

#include <iostream>
#include "thread_arg.h"

using std::cout;
using std::endl;

void* ReceivingThread(void* arg);
void* SendingThread(void* arg);

#endif // SHEDULAR_THREAD_H
