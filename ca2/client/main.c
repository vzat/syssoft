#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "macros.h"

int main (int argc, char ** argv) {
    int SID;
    int authenticated = 0;
    struct sockaddr_in server;

    char username[100];
    char password[100];
    char filename[100];
    char path[100];

    char clientMessage[MAX_BUF];
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
        if (authenticated) {
            printf("\nFilename: ");
            scanf("%s", filename);

            printf("\nPath: ");
            scanf("%s", path);

            sprintf(clientMessage, "%s\r\n%s", filename, path);

            // Send file to be transfered
            if (send(SID, clientMessage, strlen(clientMessage), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            // Receive reply from server
            if (recv(SID, serverMessage, MAX_BUF, 0) < 0) {
                printf("IO error");
                break;
            }

            printf("\n%s", serverMessage);
        }
        else {
            printf("\nUsername: ");
            scanf("%s", username);

            printf("Password: ");
            scanf("%s", password);

            sprintf(clientMessage, "%s:%s", username, password);

            // Send credentials
            if (send(SID, clientMessage, strlen(clientMessage), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            // Receive reply from server
            if (recv(SID, serverMessage, MAX_BUF, 0) < 0) {
                printf("IO error");
                break;
            }

            if (strstr(serverMessage, "authenticated")) {
                authenticated = 1;
            }
        }
    }
}
