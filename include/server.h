#ifndef SERVER_H
#define SERVER_H

void start_server(int port);
void *handle_client(void *arg);
void log_request(const char *client_ip, const char *method, const char *path);

#endif