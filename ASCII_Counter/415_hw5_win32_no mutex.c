// 415_hw4_win32.c : Defines the entry point for the console application.
//
#include <stdio.h>
#include "Windows.h"

#define SIZE 65536
#define THREAD 4
char buffer[SIZE];
int array[128];
typedef struct str_data
{
	int threadID, start, end;
}startEndPoint;

DWORD WINAPI count(LPVOID ptr)
{
	startEndPoint *data = (startEndPoint *)ptr;
	//printf("Thread ID: %d, Start: %d,  End: %d\n", data->threadID, data->start, data->end);

	for (int i = data->start; i < data->end; i++)
	{
		array[(int)buffer[i]]++;
	}

	return 0;
}

int main(int argc, char * argv[])
{
	char *filePath;
	startEndPoint data[THREAD];
	//stuff for creating and waiting functions
	HANDLE ThreadHandle[THREAD];
	DWORD ThreadID[THREAD];

	//file reading variables
	HANDLE fileHandle;
	DWORD byteRead = 0;
	int sizeDiv = 0;
	int threadCount;
	int gate = 0, loop, temp = 0;


	if (argc != 2){
		return 1;
	}
	else{
		filePath = argv[1];
		fileHandle = CreateFile((LPCTSTR)filePath,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		while (ReadFile(fileHandle, buffer, SIZE, &byteRead, NULL))
		{
			if (byteRead == 0) break; // end case that break the while-loop
			if (byteRead < THREAD)
			{
				sizeDiv = (int)byteRead;
				//printf("DADASDDA");
			}
			else
			{
				sizeDiv = (int)byteRead / THREAD;
				//sizeDiv = (int)ceil(sizeDiv);
			}
			threadCount = THREAD;
			for (int i = 0; i < threadCount; i++)
			{
				data[i].threadID = i;
				data[i].start = i * sizeDiv;

				if (byteRead < THREAD && gate == 0)
				{
					threadCount = threadCount - (THREAD - (int)byteRead);
					sizeDiv = 1;
					gate = 1;
				}
				if (i != (THREAD - 1))
				{
					data[i].end = (i + 1) * sizeDiv;
				}
				else if (i == (THREAD - 1))
				{
					data[i].end = (int)byteRead;
				}
				ThreadHandle[i] = CreateThread(NULL,
					0,
					count,
					&data[i],
					0,
					&ThreadID[i]);

			}
			for (int i = 0; i < THREAD; i++)
			{
				WaitForSingleObject(ThreadHandle[i], INFINITE);
				CloseHandle(ThreadHandle[i]);

			}
			printf("byteRead = %d, size = %d\n", byteRead, sizeDiv);
		}

		//getchar();
	/*	for (int i = 0; i <= 32; i++){
			for (loop = 0; loop < THREAD; loop++){
				temp += array[loop][i];
			}
			printf("%d occurrences of 0x%x\n", temp, i);
			temp = 0;
		}
		//print the actual char
		for (int i = 33; i< 127; i++){
			for (loop = 0; loop < THREAD; loop++){
				temp += array[loop][i];
			}
			printf("%d occurrences of '%c'\n", temp, i);
			temp = 0;
		}

		for (loop = 0; loop < THREAD; loop++){
			temp += array[loop][127];
		}
		//print the last char 'DEL'
		printf("%d occurrences of 0x%x\n", temp, 127);
		*/
		for (int i = 0; i <= 32; i++){
				temp += array[i];
			
			printf("%d occurrences of 0x%x\n", temp, i);
			temp = 0;
		}
		//print the actual char
		for (int i = 33; i< 127; i++){
				temp += array[i];

			printf("%d occurrences of '%c'\n", temp, i);
			temp = 0;
		}

			temp = array[127];
		
		//print the last char 'DEL'
		printf("%d occurrences of 0x%x\n", temp, 127);
	}



	CloseHandle(fileHandle);
	return 0;
}

