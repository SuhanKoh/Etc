
#include <stdio.h>
#include <stdlib.h>

#include <math.h> /*for pow()*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <semaphore.h>
#include <pthread.h>    /* required for pthreads */



#define BUFFER_SIZE 16
int buffer[BUFFER_SIZE];
int producer, consumer, itemCount;
int producerPtr = 0, consumerPtr = 0;
int totalConsumed = 0;
/*semaphores*/
sem_t full, empty;
pthread_mutex_t lock;
/*struct that contains info for passing into threads function*/
typedef struct str_data 
{
	int threadID;

} id;

void *Producer(void *ptr)
{
	id *data = (id *)ptr;
	int items;
	int produced = 0;

	while (produced != itemCount){
		sem_wait(&empty);
		pthread_mutex_lock(&lock);
		if (buffer[producerPtr % 16] <= 0)
		{
			items = (data->threadID*itemCount) + produced;
			buffer[producerPtr % 16] = items;
			producerPtr++;
			produced++;
		}
		pthread_mutex_unlock(&lock);
		sem_post(&full);
	}

	pthread_exit(NULL);/*(exit thread*/

}

void *Consumer(void *ptr) 
{
	int consumed = 0;
	while (consumed != ((producer*itemCount) / consumer))
	{
		sem_wait(&full);
		pthread_mutex_lock(&lock);
		if (buffer[consumerPtr % 16] >= 0)
		{
			printf("Consumed: %d\n", buffer[consumerPtr % 16]);
			buffer[consumerPtr % 16] = -1;
			consumerPtr++;
			consumed++;
			totalConsumed++;
		}
		pthread_mutex_unlock(&lock);
		sem_post(&empty);

		
	}

}


int main(int argc, const char * argv[]) {

	pthread_t *workers;
	id *data;
	/*printf("testing\n");*/
	int i;
	if (argc != 4){
		printf("Error, please check your input.\n");
		return 1;
	}
	else
	{
		producer = pow(2, atoi(argv[1]));
		consumer = pow(2, atoi(argv[2]));
		itemCount = pow(2, atoi(argv[3]));

		workers = (pthread_t *)malloc((producer*consumer)*sizeof(pthread_t));
		data = (id *)malloc((producer*consumer)*sizeof(id));

		if (pthread_mutex_init(&lock, NULL) != 0) /*ini the mutex*/
		{
			printf("Mutex failed.\n");
			return 1;
		}
		if (sem_init(&full, 0, 0) == -1){
			printf("Full Semaphore failed.\n");
			return 1;
		}
		if (sem_init(&empty, 0, 16) == -1){
			printf("Empty Semaphore failed.\n");
			return 1;
		}

		for (i = 0; i < producer; i++)
		{
			data[i].threadID = i;
			if (pthread_create(&workers[i], NULL, Producer, &data[i]) != 0)
			{
				printf("Create thread failed at producer loop.\n");
			}
		}
		for (; i < (producer + consumer); i++)
		{
			data[i].threadID = i;
			if (pthread_create(&workers[i], NULL, Consumer, &data[i]) != 0)
			{
				printf("Create thread failed at consumer loop.\n");
			}

		}

		for (i = 0; i < (producer + consumer); i++)
		{
			pthread_join(workers[i], NULL);

			/*WaitForSingleObject(threads[i], INFINITE);*/

		}
		printf("\nProducer: %d\nConsumer: %d\nItemCount(Each producer): %d\n", producer, consumer, itemCount);

		printf("Total Consumed: %d\n", totalConsumed);


	}
	pthread_mutex_destroy(&lock);
	sem_destroy(&full);
	sem_destroy(&empty);

	return 0;

}