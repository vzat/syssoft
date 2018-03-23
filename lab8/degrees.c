#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * celsius_to_fahrenheit (void * value) {
    char * temperature;
    temperature = (char *) value;

    float fahrenheit = atof(temperature);
    float celsius = fahrenheit * 9 / 5 + 32;

    printf("%.2f C = %.2f F\n", fahrenheit, celsius);
}

int main (int argc, char **argv) {
    pthread_t threads[argc];
    int iret;

    printf("Converted temperatures: \n");
    for (int i = 1 ; i < argc ; i++) {
        iret = pthread_create(&threads[i - 1], NULL, celsius_to_fahrenheit, (void *) argv[i]);

        if (iret) {
            fprintf(stderr, "Error: cannot create thread. Return code: %d\n", iret);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 1 ; i < argc ; i++) {
        pthread_join(threads[i - 1], NULL);
    }

    exit(EXIT_SUCCESS);
}
