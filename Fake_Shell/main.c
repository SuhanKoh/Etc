/*
  main.c
  415_pj7
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, const char * argv[])
{
    char *args[8];
    char buffer[1024];
    char *token;
    int tokenCount, i, j;
    pid_t pid, pid2;
    int waitGate, pipeGate;
    int status;
    int pipeID[2];
    int fd, originalFd; /* file descriptor */
    int stdGate;
    while(1)
    {
        tokenCount = 0;
        waitGate = 0;
        pipeGate = 0;
        stdGate = -1;
        /*Reseting the arrays into null for each loop*/
        args[0] = args[1] = args[2] = args[3] = args[4] = args[5] = args[6] = args[7] = NULL;
        printf("myshell> "); /**/
        if( fgets(buffer, 1024, stdin)!= NULL)
        {
            buffer[strlen(buffer)-1] = '\0'; /* remove the end \n */
            token = NULL;
            token = strtok(buffer, " "); /*get the first token, and using [space] as delimiter*/

            while(token!= NULL)
            { /* catching all the tokens. */
                args[tokenCount] = token;
                token = strtok(NULL, " ");
                tokenCount++;
            }
            if(args[0] == NULL){
                /*this if is used to prevent enter line crash.*/
            }
            else if(strcmp(args[0], "exit") == 0)
            { /* catching the "exit" from user. */
                break;
            }
            else
            {
                /**
                 * All these for-ifs are for implementing homework 7 new commandline.
                 */
                for(j = 0; j < tokenCount; j++)
                {
                    if(strcmp(args[j], ">") == 0)
                    {
                        /*Open file with "reset" each time, and set the stdout into the file*/
                        originalFd = dup(1); /*saving the original stdout*/
                        stdGate = 1;
                        fd = open(args[j+1], O_TRUNC|O_CREAT|O_RDWR, S_IRWXU);
                        args[j] = NULL; /*remove the token for > */
                        args[j+1] = NULL; /*remove the file name*/
                        dup2(fd, 1);
                        close(fd);
                        j = tokenCount;
                    }
                    else if(strcmp(args[j], ">>") == 0)
                    {
                        /*Open file for appending to stdout*/
                        originalFd = dup(1);
                        stdGate = 1;
                        fd = open(args[j+1], O_APPEND|O_CREAT|O_RDWR, S_IRWXU);
                        args[j] = NULL; /*remove the token for > */
                        args[j+1] = NULL; /*remove the file name*/
                        dup2(fd, 1);
                        close(fd);
                        j = tokenCount;
                    }else if(strcmp(args[j], "2>") == 0)
                    {
                        originalFd = dup(2);
                        stdGate = 2;
                        fd = open(args[j+1], O_TRUNC|O_CREAT|O_RDWR, S_IRWXU);
                        args[j] = NULL;
                        args[j+1] = NULL;
                        dup2(fd, 2);
                        close(fd);
                        j = tokenCount;
                    }else if(strcmp(args[j], "2>>") == 0)
                    {
                        originalFd = dup(2);
                        stdGate = 2;
                        fd = open(args[j+1], O_APPEND|O_CREAT|O_RDWR, S_IRWXU);
                        args[j] = NULL;
                        args[j+1] = NULL;
                        dup2(fd, 2);
                        close(fd);
                        j = tokenCount;
                    }else if(strcmp(args[j], "<") == 0)
                    {
                        originalFd = dup(0);
                        stdGate = 0;
                        fd = open(args[j+1], O_APPEND|O_CREAT|O_RDWR, S_IRWXU);
                        args[j] = args[j+1];
                        args[j+1] = NULL;
                        dup2(fd, 2);
                        close(fd);
                        j = tokenCount;
                    }

                    else if(strcmp(args[j], "&") == 0)
                    {
                        args[j] = NULL;
                        waitGate = 1;
                        j = tokenCount;
                    }
                    else if(strcmp(args[j], "|") == 0)
                    {
                        pipeGate = 1;
                        if(j < 3)
                        {
                            /*shifting the array into two process pipeline*/
                            for(i = 0; i < tokenCount-(j); i++)
                            {
                                args[(tokenCount+(3-j))-1 - i]  = args[(tokenCount-1)-i];
                                args[(tokenCount-1)-i]= NULL;
                            }
                        }
                        args[j] = NULL;
                        if ((status = pipe(pipeID)) == -1)
                        { /* error exit - bad pipe */
                            perror("Bad pipe");
                            return -1;
                        }

                    }

                }
                if(pipeGate == 0)
                {
                    pid = fork(); /* making a child process with pid that wait for the child to finish. */
                    if (pid == 0)
                    {
                        execlp(args[0], args[0], args[1], args[2],args[3], NULL);
                    }else
                    {
                        if(waitGate == 0)
                        {
                            wait(&pid); /* wait for child process to finish by using the pid from child process, which was returned from fork(). */
                            if(stdGate != -1)
                            {/*Change back the stdout/err/in to the original*/
                                dup2(originalFd, stdGate);
                            }
                        }
                    }

                }else
                {
                    /*For piping*/
                    pid = fork(); /*first child, right pipe execute first.*/
                    if(pid == 0)
                    {
                        close(0);
                        dup(pipeID[0]);
                        close (pipeID[0]);
                        close(pipeID[1]);
                        execlp(args[4],args[4],args[5], args[6], args[7], NULL);
                    }
                    pid2 = fork(); /*second child*/
                    if(pid2 == 0)
                    {
                        close (1);
                        dup(pipeID[1]);
                        close(pipeID[0]);
                        close(pipeID[1]);
                        execlp(args[0],args[0],args[1], args[2], args[3], NULL);
                    }
                    close(pipeID[0]);
                    close(pipeID[1]);
                    wait(&pid);
                    wait(&pid2);
                }
            }
        }
    }
    return 0;
}
