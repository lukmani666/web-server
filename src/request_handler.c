#include "request_handler.h"
#include "response.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

void handle_request(int client_fd, const char *request) {
    char method[8], path[1024];
    char body[4096];

    sscanf(request, "%s %s", method, path);

    printf("Method: %s, Path: %s\n", method, path);

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    getpeername(client_fd, (struct sockaddr *)&addr, &addr_len);
    char *client_ip = inet_ntoa(addr.sin_addr);

    log_request(client_ip, method, path);

    route_request(client_fd, method, path, request); 

    if (strcmp(method, "GET") == 0) {
        serve_file(client_fd, path);
    } else if (strcmp(method, "POST") == 0) {
        char *body_start = strstr(request, "\r\n\r\n");
        if (body_start) {
            strcpy(body, body_start + 4);
            printf("POST Body: %s\n", body);
        }
        send_post_response(client_fd, body);
    } else {
        send_405_response(client_fd);
    }  

}

void route_request(int client_fd, const char *method, const char *path, const char *request) {
    char body[4096];
    if (strcmp(path, "/") == 0 && strcmp(method, "GET") == 0) {
        serve_file(client_fd, path);
    } else if (strcmp(path, "/form") == 0 && strcmp(method, "GET") == 0) {
        serve_file(client_fd, path);
    } else if (strcmp(path, "/submit") == 0 && strcmp(method, "POST") == 0) {
        char *body_start = strstr(request, "\r\n\r\n");
        if (body_start) {
            strcpy(body, body_start + 4);
            printf("POST Body: %s\n", body);
        }
        handle_form_submission(client_fd, body);
    } else {
        send_404_response(client_fd);
    }
}

void handle_form_submission(int client_fd, const char *body) {
    char response_body[8192];

    char key[1024], value[1024];
    // char form_data[4096];
    // form_data[0] = '\0';
    char *body_copy = strdup(body);

    snprintf(response_body, sizeof(response_body),
            "<html><body>"
            "<h1>Form Submission Received</h1>");

    char *pair = strtok(body_copy, "&");

    while (pair) {
        if (sscanf(pair, "%[^=]=%s", key, value) == 2) {
            char decode_value[1024];
            url_decode(value, decode_value, sizeof(decode_value));

            printf("Form Data Received: %s=%s\n", key, value);

            snprintf(response_body + strlen(response_body), sizeof(response_body) - strlen(response_body),
                    "<p><strong>%s:</strong> %s</p>", key, decode_value);
        }
        
        pair = strtok(NULL, "&");

    } 


    snprintf(response_body + strlen(response_body), sizeof(response_body) - strlen(response_body),
            "</body></html>");

    send_response(client_fd, 200, "OK", "text/html", response_body);
    free(body_copy);
}

int hex_to_int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
}

void url_decode(const char *src, char *dest, size_t dest_size) {
    
    char *dest_ptr = dest;
    const char *src_ptr = src;
    size_t i = 0;

    while (*src_ptr && i < dest_size - 1) {
        if (*src_ptr == '%') {
            if (isxdigit(*(src_ptr + 1)) && isxdigit(*(src_ptr + 2))) {
                int high = hex_to_int(*(src_ptr +  1));
                int low = hex_to_int(*(src_ptr + 2));
                *dest_ptr = (char)((high << 4) | low);
                src_ptr += 3;
            }
        } else if (*src_ptr == '+') {
            *dest_ptr = ' ';
            src_ptr++;
        } else {
            *dest_ptr = *src_ptr;
            src_ptr++;
        }
        dest_ptr++;
        i++;
    }
    *dest_ptr = '\0';
}