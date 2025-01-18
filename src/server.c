#include "server.h"
#include "request_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 4096

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    read(client_fd, buffer, BUFFER_SIZE);

    handle_request(client_fd, buffer);

    close(client_fd);
    free(arg);

    return NULL;
}

void start_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char server_ip[INET_ADDRSTRLEN];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    if (server_addr.sin_addr.s_addr == INADDR_ANY) {
        strcpy(server_ip, "127.0.0.1");
    } else {
        inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN);
    }

    printf("Server is listening on http://%s:%d\n", server_ip, ntohs(server_addr.sin_port));

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Failed to accept connection");
            continue;
        }


        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_fd_ptr);
        pthread_detach(thread);

    }

    close(server_fd);
}

void log_request(const char *client_ip, const char *method, const char *path) {
    FILE *logfile = fopen("server.log", "a");
    if (logfile == NULL) {
        perror("Unable to open log file");
        return;
    }

    fprintf(logfile, "Client IP: %s, Method: %s, Path: %s\n", client_ip, method, path);
    fclose(logfile);
}