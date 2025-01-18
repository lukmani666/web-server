#ifndef RESPONSE_H
#define RESPONSE_H

void serve_file(int client_fd, const char *path);
void send_404_response(int client_fd);
void send_405_response(int client_fd);
void send_500_response(int client_fd);
void send_post_response(int client_fd, const char *body);
void send_response(int client_fd, int status_code, 
    const char *status_message, const char *content_type,
    const char *body
);

#endif