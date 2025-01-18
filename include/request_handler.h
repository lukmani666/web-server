#include <stddef.h>
#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void handle_request(int client_fd, const char *request);
void route_request(int client_fd, const char *method, const char *path, const char *request);
void handle_form_submission(int client_fd, const char *body);
int hex_to_int(char c);
void url_decode(const char *src, char *dest, size_t dest_size);

#endif