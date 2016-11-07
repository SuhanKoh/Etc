#include <stdlib.h>
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <string.h> /* for strcmp */
#include <unistd.h> /* for close() */
#include "structs.h"

#define RCVBUFSIZE 256 /* Size of receive buffer */

void DieWithError(char *errorMessage);

int findAuthUser(char *username, char *password, user users[], char **response);

int putMessageToUser(char *name, char *message, user users[]);

char *getMessage(char *name, user users[]);

int HandleTCPClient(int clntSocket, user users[]) {

    char *echoBuffer;
    char *valueOne;
    char *valueTwo;
    char *responseMessage;
    char *modeCode;
    int recvMsgSize;
    int i = 0;
    echoBuffer = malloc(RCVBUFSIZE);
    valueOne = malloc(RCVBUFSIZE);
    valueTwo = malloc(RCVBUFSIZE);
    responseMessage = malloc(RCVBUFSIZE);
    modeCode = malloc(RCVBUFSIZE);

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
        DieWithError("recv() failed");
    }

    echoBuffer[recvMsgSize] = '\0';
    modeCode = strsep(&echoBuffer, " ");

    if (strcmp("0", modeCode) == 0) {
        valueOne = strsep(&echoBuffer, " ");
        valueTwo = strsep(&echoBuffer, " ");

    } else if (strcmp("2", modeCode) == 0) {
        valueOne = strsep(&echoBuffer, " ");
        strcpy(valueTwo, echoBuffer);

    } else if (strcmp("3", modeCode) == 0) {
        valueOne = strsep(&echoBuffer, " ");

    } else if (strcmp("4", modeCode) == 0) {
        return 0;
    }


    if (strcmp("0", modeCode) == 0) {
        if (findAuthUser(valueOne, valueTwo, users, &responseMessage) != 1) {
            responseMessage = "Error!!\n";
        }

    } else if (strcmp("1", modeCode) == 0) {
        printf("Return use list!\n");
        for (i = 0; i < 2; i++) {
            strcat(responseMessage, users[i].username);
            strcat(responseMessage, "\n");
        }

    } else if (strcmp("2", modeCode) == 0) {
        puts(valueOne);
        puts(valueTwo);
        if (putMessageToUser(valueOne, valueTwo, users) == 1) {
            responseMessage = "Message sent successfully!\n";
        }
    } else if (strcmp("3", modeCode) == 0) {
        responseMessage = getMessage(valueOne, users);
    }


    printf("qwe: %s\n", responseMessage);
    if (send(clntSocket, responseMessage, strlen(responseMessage), 0) != strlen(responseMessage)) {
        DieWithError("send() failed");
    }

    printf("Close Sock\n");

    free(echoBuffer);


    return 1;
}

int findAuthUser(char *username, char *password, user users[], char **response) {
    int i = 0;
    int found = 0;
    printf("user: %s \t pw: %s \n", username, password);
    for (i = 0; i < 2; i++) {

        printf("compare user:%d\tcompare pw:%d\n", strncmp(users[i].username, username, sizeof(users[i].username)),
               strncmp(users[i].password, password, sizeof(users[i].password)));
        if (strncmp(users[i].username, username, sizeof(users[i].username)) == 0 &&
            strncmp(users[i].password, password, sizeof(users[i].password)) == 0) {
            /* found */
            found = 1;
            strcpy(*response, "Successfully logged in!\n");
            if (strlen(users[i].message) != 0) {
                strcat(*response, "You have a message!\n");
                puts(*response);
            }

            break;
        }
    }
    return found;
}

int putMessageToUser(char *name, char *message, user users[]) {
    int i = 0;
    int isSuccessful = 0;
    printf("A message to %s\n", name);
    printf("Message is :%s\n", message);

    for (i = 0; i < 2; i++) {
        printf("%d = i \n", i);
        if (strncmp(users[i].username, name, sizeof(users[i].username)) == 0) {
            printf("Found user %s\n", name);
            users[i].message = message;
            isSuccessful = 1;
            printf("Input:%s \n message:%s\n", message, users[i].message);
            break;
        }
    }
    return isSuccessful;

}

char *getMessage(char *name, user users[]) {
    int i = 0;
    int isSuccessful = 0;

    for (i = 0; i < 2; i++) {
        if (strncmp(users[i].username, name, sizeof(users[i].username)) == 0) {

            return users[i].message;
        }
    }
    return "No message found!";

}
