//
//  main.c
//  415_pj4
//
//  Created by Suhan Koh on 3/23/15.
//  Copyright (c) 2015 Suhan Koh. All rights reserved.
//

#include <stdio.h>
#include <pthread.h> //threads

//open file
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h> //read & write


#define THREADS 8
#define SIZE 65536
char buffer[SIZE];
int array[128];
pthread_mutex_t lock;
typedef struct str_data //struct that contains info for passing into threads function
{
    int thread, start, end;

} startEndPoint;


void *counting (void *ptr) {
    
    startEndPoint *data = (startEndPoint *) ptr; //get the struct passed by p_create()
    printf("Thread ID: %d, Start: %d,  End: %d\n", data->thread, data->start, data->end);
    //locks
    pthread_mutex_lock(&lock);
    for(int i = data->start; i < data->end; i++){
       // printf("test in counting\n");
        array[(int)buffer[i]]++; // increment by the character
    }
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);//exit thread
}


int main(int argc, const char * argv[]) {

    pthread_t workers[THREADS];
    const char *file; // path pointer
    int fd; // file descriptor
    size_t bufferReadSize; // size of the buffer read.
    int sizeDiv;
    startEndPoint data[THREADS]; // struct
    int i = 0;
    //these two variable is made to use at the adding at the printing
    int temp = 0;
    int loop, gate = 0;
    int threadCount;
    if(argc > 2 || argc < 2){ //too much input
        
        return 1;
    }else{
        file = argv[1];
        fd = open(file, O_RDONLY);
        while((bufferReadSize = read(fd, &buffer, SIZE))){
            
            if(bufferReadSize<THREADS){
                sizeDiv = (int)bufferReadSize;
            }else{
                sizeDiv = (int)bufferReadSize / THREADS;
                sizeDiv = (int)ceil(sizeDiv);
            }
            if (pthread_mutex_init(&lock, NULL) != 0)
            {
                printf("Mutex failed.\n");
                return 1;
            }
            

            //creating x threads
            threadCount = THREADS;
            for(int i = 0; i < threadCount; i++){
                data[i].thread = i;
                data[i].start = i * sizeDiv;

                //making the last thread will take the remainder of the size
                if(THREADS>bufferReadSize&&gate == 0){
                    threadCount = threadCount - (THREADS-(int)bufferReadSize);
                    sizeDiv = 1;
                    gate = 1; // making sure this will only run once since the i need to be looping.
                    
                }

                if(i != THREADS-1){
                    data[i].end = (i+1) * sizeDiv;
                }
                else if(i == THREADS-1){
                    data[i].end = (int)bufferReadSize;
                    
                }
                
                if(pthread_create(&workers[i], NULL, counting,  &data[i])!= 0){
                    printf("CREATE PROCESS FAILED!\n");
                    return 1;
                }
                
                
            }
            for(int j = 0; j < THREADS; j++){ //wait for the threads
                pthread_join(workers[j], NULL);
            }
            pthread_mutex_destroy(&lock);

        }
  /*
        //print the hex
        for(int i = 0; i <= 32; i++){
            for(loop = 0; loop < THREADS; loop++){
                temp += array[i];
            }
            printf("%d occurrences of 0x%x\n", temp, i);
            temp = 0;
        }
        //print the actual char
        for(; i< 127; i++){
            for(loop = 0; loop < THREADS; loop++){
                temp += array[i];
            }
            printf("%d occurrences of '%c'\n", temp, i);
            temp = 0;
        }
        
        for(loop = 0; loop < THREADS; loop++){
            temp += array[i];
        }
        //print the last char 'DEL'
        printf("%d occurrences of 0x%x\n", temp, i);
*/
   
        //new printing part
        for( i = 0; i <= 32; i++){
            printf("%d occurrences of 0x%x\n", array[i], i);
        }
        for(; i < 127; i++){
            printf("%d occurrences of '%c'\n", array[i], i);
        }
        printf("%d occurrences of 0x%x\n", temp, i);
    }
   
   
    printf("%d\n", array[65]);
    close(fd);
    
       return 0;
}


