#include <stdio.h> 
#include<stdlib.h> 
#include<pthread.h> 
#include<semaphore.h> 
#include "buffer.h"


#define RAND_DIVISOR 100000000 //RANDOM divisor
pthread_mutex_t mutex; // protection
sem_t empty, full; 

buffer_item buffer[BUFFER_SIZE];
//insert & remove 
int in = 0, out = 0; //tracker
int insert_item (buffer_item item){
    //insert into buffer
    if ((in + 1)% BUFFER_SIZE == out){
        //full 
        return -1; // failed to insert
    }
    buffer[in] = item; 
    in = (in +1) % BUFFER_SIZE; 
    return 0; 
}
int remove_item(buffer_item *item){
    //remove
    if(in == out){
        //buffer is empty
        return -1; 
    }
    *item = buffer[out]; 
    out = (out +1)% BUFFER_SIZE; 
    return 0; 
}


//producer th
void *produce(void *param){
    buffer_item item; 
    while(1){
        //sleep for rand pr of time
        usleep(rand() / RAND_DIVISOR);
        //gen random number
        item = rand(); 
        //aquire sem 
        sem_wait(&empty); 
        pthread_mutex_lock(&mutex); 
        if(insert_item(item)){
            fprintf(stderr,"producer failure to insert");
        }else{
            fprintf("producer produced %d\n",item); 
        }
        pthread_mutex_unlock(&mutex); 
        sem_post(&full); 
    }
}
//consumer th
void *consume(void *param){
    buffer_item item; 
    while(1){
        usleep(rand()/ RAND_DIVISOR);
        sem_wait(&full);
        pthread_mutex_lock(&mutex); 
        if(remove_item(&item)){
            fprintf(stderr,"error: consumer failed to remove");
        }else{
            printf("consumer consumed %d\n",item); 
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); 
    }
}
int main(int argc, char *argv[]){
    int sleepTime, numP, numC, i; 
    if(argc !=4 ){
        fprintf(stderr, "Usage: %s <sleep time> <# of producer threads> <# of consumer threads>\n", argv[0]);
        return -1; 
    }
    sleepTime = atoi(argv[1]); 
    numP = atoi(argv[2]); 
    numC = atoi(argv[3]); 
    pthread_mutex_init(&mutex,NULL); // create mutex lock
    //INIT semaphores
    sem_init(&empty, 0, BUFFER_SIZE); //empty
    sem_init(&full, 0, 0); // full consume// items for consumption
    //create threads
    pthread_t producers[numP]; 
    pthread_t consumers[numC];
    
    for(i = 0; i < numP; i++){
        pthread_create(&producers[i], NULL, produce, NULL); 
    }
    for(i = 0; i < numC;i++){
        pthread_create(&consumers[i],NULL, consume, NULL); 
    }
    //sleep
    sleep(sleepTime); 

    //exit
    return 0; 

}