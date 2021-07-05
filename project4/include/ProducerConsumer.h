#ifndef PRODUCERCONSUMER_H
#define PRODUCERCONSUMER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include "BoundedBuffer.h"

using namespace std;

// DO NOT modify any code here
void InitProducerConsumer(int p, int c, int psleep, int csleep, int items);
void *producer(void *);
void *consumer(void *);

#endif
