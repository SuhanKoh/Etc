#include <stdio.h>
#include <Windows.h>
#include <string.h>
int main(int argc, char *argv[]){
	char buffer[1024], inputBuffer[1024], inputBuffer2[1024];
	char *args[8];
	char *token;
	int tokenCount;
	char myExit[] = "exit";
	int i, j, ptr, isStringParsed, openFileMarker, pipeGate;
	int backgroundProcess;
	HANDLE ReadHandle, WriteHandle;

	HANDLE ogStdIn, ogStdout, ogStdErr, ogStdIn2, ogStdout2, ogStdErr2;
	HANDLE fileHandle;
	STARTUPINFO si, si2;
	PROCESS_INFORMATION pi, pi2;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	ZeroMemory(&si2, sizeof(si2));
	si2.cb = sizeof(si2);
	ZeroMemory(&pi2, sizeof(pi2));

	ogStdIn = si.hStdInput;
	ogStdout = si.hStdOutput;
	ogStdErr = si.hStdError;
	ogStdIn2 = si2.hStdInput;
	ogStdout2 = si2.hStdOutput;
	ogStdErr2 = si2.hStdError;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;


	while (1){
        //get handles of all the input output errout
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si2.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si2.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si2.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		openFileMarker = 0;
		pipeGate = 0;
		backgroundProcess = 0;
		tokenCount = 0;
		isStringParsed = 0; // mark change if we need to concat the string
        /*Reset the args*/
		args[0] = args[1] = args[2] = args[3] = args[4] = args[5] = args[6] = args[7] = NULL;
		/*End of resets*/


		printf("MyShell> ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0'; //remove the user \n
		token = NULL;
		token = strtok(buffer, " ");
		
		while (token != NULL)
		{ /* catching all the tokens. */
			args[tokenCount] = token;
			token = strtok(NULL, " ");
			tokenCount++;
		}

		if (strcmp(buffer, myExit) == 0){
			break;
		}

		else{	
			ptr = tokenCount;
			for (i = 0; i < tokenCount; i++){
				if ((strcmp(args[i], "&")) == 0){
					ptr = i; // save the limit which get into the createprocess
					backgroundProcess = 1;
				}
				else if ((strcmp(args[i], ">")) == 0){
					ptr = i; // save the limit which get into the createprocess
					fileHandle = CreateFile((LPCTSTR)args[i + 1],
									GENERIC_WRITE,
									FILE_SHARE_WRITE,
									&sa,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
					si.dwFlags = STARTF_USESTDHANDLES;

					si.hStdOutput = fileHandle;
					openFileMarker = 1;
				}
				else if ((strcmp(args[i], ">>")) == 0){
					ptr = i; // save the limit which get into the createprocess
					fileHandle = CreateFile((LPCTSTR)args[i + 1],
						FILE_APPEND_DATA,
						FILE_SHARE_WRITE,
						&sa,
						OPEN_EXISTING | CREATE_NEW,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
					si.dwFlags = STARTF_USESTDHANDLES;

					si.hStdOutput = fileHandle;
					openFileMarker = 1;
				}
				else if ((strcmp(args[i], "2>")) == 0){
					ptr = i; // save the limit which get into the createprocess
					fileHandle = CreateFile((LPCTSTR)args[i + 1],
						GENERIC_WRITE,
						FILE_SHARE_WRITE,
						&sa,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
					si.dwFlags = STARTF_USESTDHANDLES;
					si.hStdError = fileHandle;
					openFileMarker = 1;
				}
				else if ((strcmp(args[i], "2>>")) == 0){
					ptr = i; // save the limit which get into the createprocess
					fileHandle = CreateFile((LPCTSTR)args[i + 1],
						FILE_APPEND_DATA,
						FILE_SHARE_WRITE,
						&sa,
						OPEN_EXISTING | CREATE_NEW,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
					si.dwFlags = STARTF_USESTDHANDLES;
					si.hStdError = fileHandle;
					openFileMarker = 1;
				}
				else if ((strcmp(args[i], "<")) == 0){
					ptr = i; // save the limit which get into the createprocess
					fileHandle = CreateFile((LPCTSTR)args[i + 1],
						GENERIC_READ,
						FILE_SHARE_READ,
						&sa,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
					si.dwFlags = STARTF_USESTDHANDLES;
					si.hStdInput = fileHandle;
					si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
					si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

					openFileMarker = 1;


				}
				else if ((strcmp(args[i], "|")) == 0){
					pipeGate = 1;
					ptr = i;
					args[i] = " ";
					if (!CreatePipe(&ReadHandle, &WriteHandle, &sa, 0)){
						fprintf(stderr, "Create Pipe Failed\n");
						return 1;
					}
				}
			}
			if (pipeGate == 0){
				strcpy(inputBuffer, args[0]);
				strcat(inputBuffer, " ");
				for (i = 1; i < ptr; i++){
					strcat(inputBuffer, args[i]);
					strcat(inputBuffer, " ");
				}

				//printf("%s \n", inputBuffer);

				CreateProcess(NULL,
					inputBuffer,
					NULL,
					NULL,
					TRUE,
					0,
					NULL,
					NULL,
					&si,
					&pi);

			}
			else if(pipeGate == 1){
				strcpy(inputBuffer, args[0]);
				strcat(inputBuffer, " ");
				for (i = 1; i < ptr; i++){
					strcat(inputBuffer, args[i]);
					strcat(inputBuffer, " ");
				}
				i++;

				strcpy(inputBuffer2, args[i]);
				strcat(inputBuffer2, " ");
				
				for (; i < tokenCount-1; i++){
					strcat(inputBuffer2, args[i]);
					strcat(inputBuffer2, " ");
				}
				
				printf("1st: %s \n", inputBuffer);
				printf("2nd: %s \n", inputBuffer2);
				si.dwFlags = STARTF_USESTDHANDLES;
				si.hStdInput = ReadHandle;
				si2.dwFlags = STARTF_USESTDHANDLES;
				si2.hStdOutput = WriteHandle;

				si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
				si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
				si.hStdInput = ReadHandle;
				si.dwFlags = STARTF_USESTDHANDLES;

				CreateProcess(NULL,
					inputBuffer2,
					NULL,
					NULL,
					TRUE,
					0,
					NULL,
					NULL,
					&si,
					&pi);

				si2.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
				si2.hStdError = GetStdHandle(STD_ERROR_HANDLE);
				si2.hStdOutput = WriteHandle;
				si2.dwFlags = STARTF_USESTDHANDLES;

				CreateProcess(NULL,
					inputBuffer,
					NULL,
					NULL,
					TRUE,
					0,
					NULL,
					NULL,
					&si2,
					&pi2);
				
				WaitForSingleObject(pi2.hProcess, INFINITE);
				WaitForSingleObject(pi.hProcess, INFINITE);
				backgroundProcess = 1;
			}
			if (openFileMarker != 0){
				CloseHandle(fileHandle);
			}
		}
		if (backgroundProcess == 0){
			WaitForSingleObject(pi.hProcess, INFINITE);
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 1;
}