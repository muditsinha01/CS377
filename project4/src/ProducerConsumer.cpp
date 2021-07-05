#include "ProducerConsumer.h"

// TODO: add BoundedBuffer, locks and any global variables here
static BoundedBuffer *buffer(0);
static int producer_num;
static int consumer_num;
static int psleep_time;
static int csleep_time;
static int item_limit;
static ofstream outfile;
static pthread_mutex_t lock;

void InitProducerConsumer(int p, int c, int psleep, int csleep, int items) {
  // TODO: constructor to initialize variables declared
  //       also see instructions on the implementation
	buffer = new BoundedBuffer(items);
	producer_num = 0;
	consumer_num = 0;
	psleep_time = psleep;
	csleep_time = csleep;
	item_limit = items;
	pthread_mutex_init(&lock, NULL);

	outfile.open("output.txt");

	pthread_t producers[p];
	pthread_t consumers[c];
	for(int i = 0; i < p; i++){
		pthread_create(&producers[i], NULL, producer, (void *)&i);
	}
	for(int j = 0; j < c; j++){
		pthread_create(&consumers[j], NULL, consumer, (void *)&j);
	}

	while(consumer_num < item_limit){
		;
	}

	pthread_mutex_destroy(&lock);
	pthread_exit(NULL);
}

void* producer(void* threadID) {
  // TODO: producer thread, see instruction for implementation
	float s = psleep_time/1000;
   	int tid = *((int*)threadID);
	while(true){
		sleep(s);
		pthread_mutex_lock(&lock);
		if(producer_num < item_limit){
			cout<<producer_num<<endl;
			int add = rand();
			buffer->append(add);
			time_t my_time = time(NULL);
			outfile << "Producer " << tid << ", time = " << ctime(&my_time) << ", producing data item " << producer_num << ", item value=" << add << endl;
			producer_num++;
		}
		else{
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(NULL);
}

void* consumer(void* threadID) {
  // TODO: consumer thread, see instruction for implementation
	float s = csleep_time/1000;
   	int tid = *((int*)threadID);
	while(true){
		sleep(s);
		pthread_mutex_lock(&lock);
		if(consumer_num < item_limit){
			int res = buffer->remove();
			time_t my_time = time(NULL);
			outfile << "Consumer " << tid << ", time = " << ctime(&my_time) << ", consuming data item with value=" << res << endl;
			consumer_num++;
		}
		else{
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(NULL);
}
