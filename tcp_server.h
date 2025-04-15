#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 1024
#define LONGER_BUFFER_SIZE 2048

int createSocket(int port);
void acceptConnections(int listening_socket);
void handleConnection(int a_client);

#endif  // TCP_SERVER_H
