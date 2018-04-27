// Vlad Zat
// C14714071
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "connection.h"

int main (int argc, char **argv) {
    int s, cs, connSize;

    struct sockaddr_in server, client;

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf("Could not create socket\n");
    }
    else {
        printf("Socket successfully created\n");
    }

    // Init socket
    server.sin_port = htons(8081);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Bind issue");
        return 1;
    }
    else {
        printf("Bind complete\n");
    }

    // Listen for connection
    listen(s, 3);

    // Accept incoming connections
    printf("Waiting for incoming connections from client\n");
    connSize = sizeof(struct sockaddr_in);

    // Accept a connection
    while((cs = accept(s, (struct sockaddr *) &client, (socklen_t*) &connSize))) {
        printf("Connection from client accepted\n");

        pthread_t client_thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, 1);

        int rc;

        if ((rc = pthread_create(&client_thread, &attr, connection, (void*) &cs))) {
            printf("Error creating thread");
            return EXIT_FAILURE;
        }
  }

  if (cs < 0) {
      perror("Cannot establish connection\n");
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
