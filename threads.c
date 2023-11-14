#include <stdio.h> 
#include<stdlib.h> 
#include<pthread.h> 
#include<semaphore.h> 
#include "buffer.h"


#define RAND_DIVISOR 324234 //RANDOM divisor
pthread_mutex_t mutex; //
sem_t empty, full; // empty to count num of empty slots in buffer. 

buffer_item buffer[BUFFER_SIZE];
//insert & remove 
int in = 0, out = 0; //tracker
int insert_item (buffer_item item){
    if ((in + 1)% BUFFER_SIZE == out){   // check if full 
        //full `
        fprintf(stderr,"buffer is full\n"); 
        return -1;                      // failed to insert
    }
    buffer[in] = item;                  // insert item 
    in = (in +1) % BUFFER_SIZE;         // inc and wrap 
    return 0; 
}
int remove_item(buffer_item *item){
    if(in == out){
        //buffer is empty
        fprintf(stderr,"buffer is empty"); 
        return -1; 
    }
    *item = buffer[out]; 
    out = (out +1)% BUFFER_SIZE; //increment and wrap around
    return 0; 
}
//producer th
void *produce(void *param){
    buffer_item item; 
    while(1){
        //sleep for rand pr of time in order to produce at random interval
        usleep(rand() / RAND_DIVISOR);
        //gen random number
        item = rand(); 
        //aquire sem 
        sem_wait(&empty);                   //wait for empty spot
        //protect critical 
        pthread_mutex_lock(&mutex); 
        if(insert_item(item)){
            fprintf(stderr,"producer failure to insert\n");
        }else{
            printf("producer produced %d\n",item); 
        }
        pthread_mutex_unlock(&mutex);       //unlock the mutex after exiting 
                                            //item produced
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
            fprintf(stderr,"error: consumer failed to remove\n");
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
        fprintf(stderr, "Usage: %s <sleep time> <# of producer threads> <# of consumer threads>\n", argv[0]);//checkcli
        return -1; 
    }
    sleepTime = atoi(argv[1]); 
    numP = atoi(argv[2]);       //convert arg string to ints
    numC = atoi(argv[3]); 
    pthread_mutex_init(&mutex,NULL); 
    //INIT semaphores
    sem_init(&empty, 0, BUFFER_SIZE); //empty
    sem_init(&full, 0, 0); // full consume// items for consumption
    //create threads
    pthread_t producers[numP]; 
    pthread_t consumers[numC];
    //start routine
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