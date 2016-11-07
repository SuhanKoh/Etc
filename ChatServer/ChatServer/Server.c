//
//  main.c
//  Chat
//
//  Created by Suhan Koh on 3/7/16.
//  Copyright © 2016 Suhan Koh. All rights reserved.
//

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "structs.h"

#include <stdio.h>

#define ADDRESS "127.0.0.1"
#define PORT 8000

void DieWithError(char *errorMessage);

/* Error handling function */
int HandleTCPClient(int clientSocket, user users[]);

int main(int argc, const char *argv[]) {

    int serverSock; /* Socket descriptor for server */
    int clientOneSock; /* Socket descriptor for client 1 */

    unsigned int clientLength;
    struct sockaddr_in serverAddr; /* Local address */

    struct sockaddr_in clientAddr1;
    user users[] = {
            {"Alice", "1234",   0, ""},
            {"Bob",   "123456", 0, ""}
    };

    memset(&serverAddr, 0, sizeof(serverAddr)); /* Zero out structure */
    serverAddr.sin_family = AF_INET; /* Internet address family */
    serverAddr.sin_addr.s_addr = inet_addr(ADDRESS); /* Any incoming interface */
    serverAddr.sin_port = htons(PORT); /* Local port */

    if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("Socket failed!");
    }

    if (bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        DieWithError("bind () failed");
    }

    printf("Server Started!\n");
    printf("Listening on %s:%d\n", ADDRESS, PORT);
    if (listen(serverSock, 2) < 0) {
        DieWithError("listen() failed");
    }


    while (1) {
        clientLength = sizeof(&clientAddr1);
        if ((clientOneSock = accept(serverSock, (struct sockaddr *) &clientAddr1, &clientLength)) < 0) {
            DieWithError("accept() failed");
        }
        printf("Client Connected: %s\n", inet_ntoa(clientAddr1.sin_addr));

        while (1) {

            if (HandleTCPClient(clientOneSock, users) == 0) {
                printf("BROKE LOL\n");
                close(clientOneSock);
                break;
            }

        }
    }
    return 0;
}

