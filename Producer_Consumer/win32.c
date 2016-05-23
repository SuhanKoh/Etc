// 415_hw5_win32_pt2.c
//

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>



#define BUFFER_SIZE 16
//Global Variables
int  buffer[BUFFER_SIZE];
HANDLE full, empty;
HANDLE myMutex;


int power[3]; //used to contain the input
int producer, consumer, itemCount;
int producerPtr = 0, consumerPtr = 0, totalConsumed = 0;

struct str_data
{
	int threadID;
}id;

DWORD WINAPI Producer(LPVOID ptr)
{
	int items;
	struct str_data *data = (struct str_data *)ptr;
	int produced = 0;
	while (produced < itemCount)
	{
		WaitForSingleObject(empty, INFINITE);
		WaitForSingleObject(myMutex, INFINITE);

		if (buffer[producerPtr % 16] <= 0) // busy waiting
		{
			items = (data->threadID * itemCount) + produced;
			buffer[producerPtr%BUFFER_SIZE] = items;
			producerPtr++;
			produced++;
		}
		ReleaseSemaphore(myMutex, 1, 0);
		ReleaseSemaphore(full, 1, 0);
	}
	return 0;
}

DWORD WINAPI Consumer(LPVOID ptr)
{
	int consumed = 0;
	while (consumed != ((producer*itemCount) / consumer))
	{
		WaitForSingleObject(full, INFINITE);
		WaitForSingleObject(myMutex, INFINITE);
		if (buffer[consumerPtr % BUFFER_SIZE] >= 0)
		{
			printf("Consumed: %d\n", buffer[consumerPtr % 16]);
			buffer[consumerPtr % BUFFER_SIZE] = -1; //marking it is OK to rewrite
			consumerPtr++; // increment the pointer
			consumed++; //increment the consumer thread consumed
			totalConsumed++; // to count how many were consumed
		}
		ReleaseSemaphore(myMutex, 1, 0);
		ReleaseSemaphore(empty, 1, 0);
	}
	//printf("Consumer exit!!!\n");
	return 0;
}

int main(int argc, char * argv[])
{
	HANDLE *threads;
	int i, j, total;
	struct str_data *data;
	if (argc != 4)
	{
		printf("ERROR, CHECK INPUT COMMANDS\n");
		return 1;
	}
	else
	{

		producer = pow(2, atoi(argv[1]));
		consumer = pow(2, atoi(argv[2]));
		itemCount = pow(2, atoi(argv[3]));
		total = producer + consumer;

		threads = (HANDLE *)malloc(total*sizeof(HANDLE));
		data = (struct str_data *)malloc(total*sizeof(struct str_data));

		/*Semaphores*/
		myMutex = CreateSemaphore(0, 1, 1, 0); // initial 1 and max 1, since wait() is S--; binary semaphore
		if (myMutex == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return 1;
		}
		full = CreateSemaphore(0, 0, BUFFER_SIZE, 0);
		if (full == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return 1;
		}
		empty = CreateSemaphore(0, BUFFER_SIZE, BUFFER_SIZE, 0);
		if (empty == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return 1;
		}
		/*End of Semaphroes*/

		for (i = 0; i < producer; i++)
		{
			data[i].threadID = i;
			threads[i] = CreateThread(NULL, 0, Producer, &data[i], 0, NULL);
		}
		for (; i < (total); i++)
		{
			data[i].threadID = i;
			threads[i] = CreateThread(NULL, 0, Consumer, &data[i], 0, NULL);

		}
		for (j = 0; j < (total); j++)
		{
			//printf("checker:%d\n", j);

			WaitForSingleObject(threads[j], INFINITE);
		}

		printf("\nProducer: %d\nConsumer: %d\nItem Produce(Each producer): %d\n", producer, consumer, itemCount);
		printf("Total Consumed: %d\n", totalConsumed);

	}
	free(threads);
	free(data);
	CloseHandle(full);
	CloseHandle(empty);
	CloseHandle(myMutex);
	//getchar();
	return 0;
}