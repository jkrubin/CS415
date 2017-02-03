#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>
#define buffLen 16

	//	SHARED DATA		//
int buffer[buffLen]={0};
int num_Produced;
int num_Consumed;
int items;
pthread_mutex_t bufferLock;
sem_t *fullSlots;
sem_t *emptySlots;
const char *full = "fullSlots";
const char *empty= "emptySlots";

	//	Function Heads	//
void *produce(void *param); //producer threads call this function
void *consume(void *param); //consumer threads call this function

int main(int argc, char *argv[]){
	int producerNum;
	int consumerNum;
	int itemNum;
	int i=0;
	
	if (argc !=4){
		printf("uses 3 args: log of producer, consumer, items");
		return -1;
	}
	producerNum= pow(2,atoi(argv[1]));
	consumerNum= pow(2,atoi(argv[2]));
	itemNum= 	 pow(2,atoi(argv[3]));
	num_Produced = itemNum/producerNum;
	num_Consumed = itemNum/consumerNum;
	
	printf("Producers: %d\nConsumers: %d\nitems:%d\n",producerNum,consumerNum,itemNum);
	
	//Init mutex and semaphores
	pthread_mutex_init(&bufferLock,NULL);
	
	sem_unlink(full);
	sem_unlink(empty);
	if((fullSlots = sem_open(full,O_CREAT,0644,0))==SEM_FAILED){
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
	if((emptySlots = sem_open(empty,O_CREAT,0644,buffLen))==SEM_FAILED){
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
	/*if(sem_init(&fullSlots,0,0)==-1&&sem_init(&emptySlots,0,16)==-1){
		printf("semaphore err\n");
		return -1;
	}*/
	
	items=0;
	
	pthread_t producers[producerNum]; // producer threads
	pthread_t consumers[consumerNum]; //consumer threads
	pthread_attr_t attr; //thread attributes
	pthread_attr_init(&attr); //instantiate attributes
	
	for(i=0;i<producerNum;i++){//create producer threads
		//printf("*****creating thread %d*******\n",i);
		pthread_create(&producers[i],&attr,produce,(void*)(i)); 
	}
	for(i=0;i<consumerNum;i++){//create consumer threads
		//printf("^^^^^^creating thread %d^^^^^^\n",i);
		pthread_create(&consumers[i],&attr,consume,(void*)(i)); 
	}
	
	//Join threads				
	for(i=0;i< producerNum;i++){ //join all threads together
		pthread_join(producers[i],NULL);
	}
	for(i=0;i< consumerNum;i++){ //join all threads together
		pthread_join(consumers[i],NULL);
	}
	return 0;
}

void *produce(void *param){
	int thread_ID;
	int i;
	thread_ID = (int)param;
	
	for(i=0;i<num_Produced;i++){
		//Wait for an empty slot and wait for buffer access
		sem_wait(emptySlots);
		pthread_mutex_lock(&bufferLock);
		// CRITICAL SECTION
		buffer[items]= ((thread_ID * num_Produced) + i);
		//printf("thread %d to slot %d\n",thread_ID,items);
		items+=1;
		//give up mutex, add to full slot semaphore
		pthread_mutex_unlock(&bufferLock);
		sem_post(fullSlots);
		
	}
	pthread_exit(0);
}

void *consume(void *param){
	int i;
	int printval;
	
	for(i=0;i<num_Consumed;i++){
		//wait for a full slot and wait for buffer access
		sem_wait(fullSlots);
		pthread_mutex_lock(&bufferLock);
		//CRITICAL SECTION
		printval=buffer[items-1];
		printf("item %d from slot %d\n",printval,(items-1));
		items+=-1;
		//give up mutex and add to empty semaphore 
		pthread_mutex_unlock(&bufferLock);
		sem_post(emptySlots);
	}
	pthread_exit(0);
}