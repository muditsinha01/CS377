#include "BoundedBuffer.h"

BoundedBuffer::BoundedBuffer(int N) {
  // TODO: constructor to initiliaze all the varibales declared in
	buffer = new int[N];
	buffer_size = N;
	buffer_cnt = 0;
	buffer_first = 0;
	buffer_last = 0;
	pthread_mutex_init(&buffer_lock, NULL);
	pthread_cond_init (&buffer_not_full, NULL);
	pthread_cond_init (&buffer_not_empty, NULL);
}

BoundedBuffer::~BoundedBuffer() {
  // TODO: destructor to clean up anything necessary
	buffer = NULL;
	buffer_size = 0;
	buffer_cnt = 0;
	buffer_first = 0;
	buffer_last = 0;
	pthread_mutex_destroy(&buffer_lock);
	pthread_cond_destroy (&buffer_not_full);
	pthread_cond_destroy (&buffer_not_empty);
}

void BoundedBuffer::append(int data) {
  // TODO: append a data item to the circular buffer
	if(buffer_cnt >= buffer_size){
		pthread_cond_wait(&buffer_not_full, &buffer_lock);
	}
	pthread_mutex_lock(&buffer_lock);
	buffer[buffer_last] = data;
	buffer_cnt++;
	buffer_last++;
	if(buffer_last >= buffer_size){
		buffer_last = 0;
	}
	pthread_cond_signal(&buffer_not_empty);
	pthread_mutex_unlock(&buffer_lock);
}

int BoundedBuffer::remove() {
  // TODO: remove and return a data item from the circular buffer
	if(buffer_cnt <= 0){
		pthread_cond_wait(&buffer_not_empty, &buffer_lock);
	}
	pthread_mutex_lock(&buffer_lock);
	int res = buffer[buffer_first];
	buffer[buffer_first] = NULL;
	buffer_cnt--;
	buffer_first++;
	if(buffer_first >= buffer_size){
		buffer_first = 0;
	}
	pthread_cond_signal(&buffer_not_full);
	pthread_mutex_unlock(&buffer_lock);
	return res;
}

bool BoundedBuffer::isEmpty() {
  pthread_mutex_lock(&buffer_lock);
  bool res = buffer_cnt <= 0;
  pthread_mutex_unlock(&buffer_lock);
  return res;
}
