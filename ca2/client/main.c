#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "macros.h"

int main (int argc, char ** argv) {
    int SID;
    struct sockaddr_in server;
    char username[100];
    char password[100];
    char credentials[MAX_BUF];
    char serverMessage[MAX_BUF];

    // Create Socket
    SID = socket(AF_INET, SOCK_STREAM, 0);
    if (SID == -1) {
        printf("Error creating socket\n");
    }
    else {
        printf("Socket created\n");
    }

    // Init Socket
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;

    if (connect(SID, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Successfully connected to the server\n");

    while (1) {
        printf("\nUsername: ");
        scanf("%s", username);

        printf("\nPassword: ");
        scanf("%s", password);

        sprintf(credentials, "%s:%s", username, password);

        // Send some data
        if (send(SID, credentials, strlen(credentials), 0) < 0) {
            printf("Send failed");
            return 1;
        }

        // Receive reply from server
        if (recv(SID, serverMessage, MAX_BUF, 0) < 0) {
            printf("IO error");
            break;
        }

        printf("Server sent: %s", serverMessage);
    }
}
