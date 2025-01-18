#include "response.h"
#include "mime_types.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


void serve_file(int client_fd, const char *path) {
    char full_path[1024];
    char buffer[1024];
    struct stat file_stat;
    int file_fd;

    if (strcmp(path, "/") == 0) {
        path = "index.html";
    } else if (strcmp(path, "/form") == 0) {
        path = "form.html";
    }

    snprintf(full_path, sizeof(full_path), "www/%s", path);

    printf("Serving file from path: %s\n", full_path);

    if (stat(full_path, &file_stat) == -1) {
        send_404_response(client_fd);
        return;
    }

    file_fd = open(full_path, O_RDONLY);
    if (file_fd == -1) {
        send_404_response(client_fd);
        return;
    }

    const char *mime_type = get_mime_type(full_path);

    snprintf(buffer, sizeof(buffer),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n\r\n",
            mime_type, file_stat.st_size);
    write(client_fd, buffer, strlen(buffer));

    ssize_t bytes_read;

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        write(client_fd, buffer, bytes_read);
    }

    close(file_fd);
}

void send_404_response(int client_fd) {
    const char *response = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 46\r\n\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>";
    write(client_fd, response, strlen(response));
}

void send_405_response(int client_fd) {
    const char *response = 
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 58\r\n\r\n"
        "<html><body><h1>405 Method Not Allowed</h1></body></html>";
    write(client_fd, response, strlen(response));
}

void send_500_response(int client_fd) {
    const char *response =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 56\r\n\r\n"
        "<html><body><h1>500 Internal Server Error</h1></body></html>";
    write(client_fd, response, strlen(response));
}

void send_post_response(int client_fd, const char *body) {
    char buffer[1024];

    snprintf(buffer, sizeof(buffer),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %ld\r\n\r\n"
            "Received POST data: %s", strlen(body), body);

    write(client_fd, buffer, strlen(buffer));
}

void send_response(int client_fd, int status_code, 
    const char *status_message, const char *content_type,
    const char *body
) {
    char response[8192];
    int content_lenght = strlen(body);

    snprintf(response, sizeof(response),
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s", 
            status_code, status_message, content_type, content_lenght, body);
    write(client_fd, response, strlen(response));

    close(client_fd);
}