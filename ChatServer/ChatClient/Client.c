//
//  main.c
//  ChatClient
//
//  Created by Suhan Koh on 3/7/16.
//  Copyright © 2016 Suhan Koh. All rights reserved.
//

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 256 /* Size of receive buffer */

void DieWithError(char *errorMessage);

/* Error handling function */
void printCommandOptions(int *option);

char *sendCommand(int socket, char *message);

int main(int argc, char *argv[]) {
    int userOption = 0;

    int sock;                           /* Socket descriptor */
    struct sockaddr_in echoServAddr;    /* Echo server address */
    unsigned short serverPortNumber;    /* Echo server port */
    char *serverlPAddress;              /* Server IP address (dotted quad) */
    char *messageToBeSend;                   /* String to send to echo server */
    char *input;
    char serverResponseCode[256];
    char *charptr;
    char userMessage[256];
    char *user;
    char *echoBuffer;

    int recvMsgSize;
    unsigned int clientLength;
    struct sockaddr_in clientAddr1;
    int clientOneSock; /* Socket descriptor for client 1 */

    serverlPAddress = malloc(RCVBUFSIZE);
    messageToBeSend = malloc(RCVBUFSIZE);
    input = malloc(RCVBUFSIZE);
    user = malloc(RCVBUFSIZE);
    echoBuffer = malloc(RCVBUFSIZE);


    while (1) {
        printCommandOptions(&userOption); /* Print the user options and then change the user option variable */
        serverResponseCode[0] = 0;
        switch (userOption) {
            case 0:
            case 5:
                printf("Please enter the IP address: ");
                scanf("%255s", serverlPAddress);
                printf("Please enter the port number: ");
                scanf("%hu", &serverPortNumber);

                break;

            default:
                break;
        }


        switch (userOption) {
            case 0:
                if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                    DieWithError(" socket () failed");
                }

                /* Construct the server address structure */
                memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
                echoServAddr.sin_family = AF_INET; /* Internet address family */
                echoServAddr.sin_addr.s_addr = inet_addr(serverlPAddress); /* Server IP address */
                echoServAddr.sin_port = htons(serverPortNumber); /* Server port */

                /* Establish the connection to the echo server */
                if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
                    DieWithError(" connect () failed");
                } else {
                    printf("Connected! Please log in.\n\n");
                }
                strcpy(messageToBeSend, "0 ");

                printf("Enter your user name: ");
                scanf("%255s", input);
                strcpy(user, input);
                strcat(messageToBeSend, input);
                strcat(messageToBeSend, " ");

                printf("Enter your password: ");
                scanf("%255s", input);
                strcat(messageToBeSend, input);

                sendCommand(sock, messageToBeSend);

                if (recv(sock, &serverResponseCode, RCVBUFSIZE - 1, 0) > 0) {
                    printf("%s\n", serverResponseCode);
                    if (strcmp("Error!!\n", serverResponseCode) == 0) {
                        close(sock);
                    }
                }


                break;

            case 1:

                sendCommand(sock, "1");

                if (recv(sock, &serverResponseCode, RCVBUFSIZE - 1, 0) > 0) {
                    printf("Users:\n%s\n", serverResponseCode);
                }

                break;

            case 2:

                strcpy(messageToBeSend, "2 ");

                printf("Enter the user name: ");
                scanf("%255s", input);
                strcat(messageToBeSend, input);
                strcat(messageToBeSend, " ");

                getchar(); //special case for using fgets after scanf

                printf("Enter your message: ");
                fgets(userMessage, sizeof(userMessage), stdin);
                charptr = userMessage;
                strcat(messageToBeSend, charptr);

                sendCommand(sock, messageToBeSend);

                if (recv(sock, &serverResponseCode, RCVBUFSIZE - 1, 0) > 0) {
                    printf("%s\n", serverResponseCode);
                }

                break;

            case 3:

                strcpy(messageToBeSend, "3 ");
                strcat(messageToBeSend, user);

                sendCommand(sock, messageToBeSend);

                if (recv(sock, &serverResponseCode, RCVBUFSIZE - 1, 0) > 0) {
                    printf("%s\n", serverResponseCode);
                }

                break;

            case 4:
                sendCommand(sock, "4");

                close(sock);

                printf("Please enter the port number you want to listen on: ");
                scanf("%hu", &serverPortNumber);

                memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
                echoServAddr.sin_family = AF_INET; /* Internet address family */
                echoServAddr.sin_addr.s_addr = inet_addr(serverlPAddress); /* Any incoming interface */
                echoServAddr.sin_port = htons(serverPortNumber); /* Local port */

                if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                    DieWithError("Socket failed!");
                }

                if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
                    DieWithError("bind () failed");
                }

                printf("Server Started!\n");
                printf("I am listening on %s:%d\n", serverlPAddress, serverPortNumber);

                if (listen(sock, 2) < 0) {
                    DieWithError("listen() failed");
                }
                clientLength = sizeof(&clientAddr1);
                if ((sock = accept(sock, (struct sockaddr *) &clientAddr1, &clientLength)) < 0) {
                    DieWithError("accept() failed");
                }
                printf("Bob Connected: %s\n", inet_ntoa(clientAddr1.sin_addr));
                getchar(); //special case for using fgets after scanf

                while (1) {
                    printf("(Type \"Bye\" to end the conversation)\n");

                    if ((recvMsgSize = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
                        DieWithError("recv() failed");
                    }

                    echoBuffer[recvMsgSize] = '\0';
                    printf("Bob: %s", echoBuffer);
                    if (strcmp("Bye\n", echoBuffer) != 0) {

                        printf("%s: ", user);

                        fgets(userMessage, sizeof(userMessage), stdin);
                        charptr = userMessage;
                        strcpy(messageToBeSend, charptr);

                        sendCommand(sock, messageToBeSend);

                    } else {
                        printf("Disconnected from your friend!");
                        close(sock);
                        break;
                    }
                    if (strcmp("Bye\n", messageToBeSend) == 0) {
                        printf("Disconnected from your friend!");
                        close(sock);
                        break;
                    }
                    memset(userMessage, 0, sizeof userMessage);

                }

                break;

            case 5:

                sendCommand(sock, "4");

                close(sock);

                if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                    DieWithError(" socket () failed");
                }

                /* Construct the server address structure */
                memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
                echoServAddr.sin_family = AF_INET; /* Internet address family */
                echoServAddr.sin_addr.s_addr = inet_addr(serverlPAddress); /* Server IP address */
                echoServAddr.sin_port = htons(serverPortNumber); /* Server port */
                printf("Connecting to your friend......\n");
                /* Establish the connection to the echo server */
                if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
                    DieWithError(" connect () failed");
                } else {
                    printf("Connected!\n\n");
                }
                getchar(); //special case for using fgets after scanf

                while (1) {
                    printf("(Type \"Bye\" to end the conversation)\n");
                    printf("%s: ", user);

                    fgets(userMessage, sizeof(userMessage), stdin);
                    charptr = userMessage;
                    strcpy(messageToBeSend, charptr);

                    sendCommand(sock, messageToBeSend);

                    if (strcmp("Bye\n", messageToBeSend) == 0) {
                        printf("Disconnected from your friend!");

                        close(sock);
                        break;
                    }

                    //Wait for alice to response
                    if ((recvMsgSize = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
                        DieWithError("recv() failed");
                    }

                    echoBuffer[recvMsgSize] = '\0';
                    printf("Alice: %s", echoBuffer);
                    if(strcmp("Bye\n", echoBuffer) == 0){
                        printf("Disconnected from your friend!");
                        close(sock);
                        break;
                    }
                    memset(userMessage, 0, sizeof userMessage);

                }

                break;
            default:
                break;
        }
        memset(serverResponseCode, 0, sizeof serverResponseCode);

    }

}

void printCommandOptions(int *option) {
    printf("-------------------------------\n");
    printf("Command:\n0. Connect to the server\n1. Get the user list\n2. Send a message\n3. Get my messages\n4. Initial a chat with my friend\n5. Chat with my friend\nYour option(enter a number): ");
    scanf("%1d", option);
}

char *sendCommand(int socket, char *message) {
    char *returnedMessage;
    int receivedSize = 0;
//    printf("sending %s \t %d \n", message, strlen(message));
    if (send(socket, message, strlen(message), 0) != strlen(message)) {
        DieWithError("send() sent a different number of bytes than expected");
    }

    if (receivedSize != 0) {
        return returnedMessage;
    } else {
        return "";
    }
}

