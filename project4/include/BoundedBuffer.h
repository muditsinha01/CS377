#ifndef _BOUNDEDBUFFER_H
#define _BOUNDEDBUFFER_H

//// DO NOT MODIFY ANYTHING IN THIS FILE //////////////////////////////////////

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class BoundedBuffer {
 public:
  BoundedBuffer(
      int N);  // constructor to initialize locks and conditional variables
  ~BoundedBuffer();  // destructor
  void append(int data);
  int remove();
  bool isEmpty();

 private:
  int *buffer;
  int buffer_size;
  int buffer_cnt;
  int buffer_first;
  int buffer_last;

  pthread_mutex_t buffer_lock;      // lock
  pthread_cond_t buffer_not_full;   // Condition indicating buffer is not full
  pthread_cond_t buffer_not_empty;  // Condition indicating buffer is not empty
};

#endif
