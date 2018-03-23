#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

float celsius_to_fahrenheit (float celsius) {
    float fahrenheit = celsius * 9 / 5 + 32;

    return fahrenheit;
}

int main (int argc, char **argv) {
    int s, cs, connSize, READSIZE;

    struct sockaddr_in server, client;
    char message[500];

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
    cs = accept(s, (struct sockaddr *) &client, (socklen_t*) &connSize);
    if (cs < 0) {
        perror("Cannot establish connection\n");
        return 1;
    }
    else {
        printf("Connection from client accepted\n");
    }

    while (1) {
        memset(message, 0, 500);
        READSIZE = recv(cs, message, 2000, 0);

        if (READSIZE == 0) {
            printf("Client disconected\n");
            fflush(stdout);
            return (EXIT_SUCCESS);
        }
        else if (READSIZE == -1) {
            perror("Read error");
        }

        float celsius = atof(message);
        float fahrenheit = celsius_to_fahrenheit(celsius);

        char retTemp[100];
        sprintf(retTemp, "%.2f C = %.2f F\n", celsius, fahrenheit);
        write(cs, retTemp, strlen(retTemp));
    }
}
