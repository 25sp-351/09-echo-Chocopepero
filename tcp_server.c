#include "tcp_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern int print_flag;
void *handleConnectionThread(void *arg);

int createSocket(int port) {
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in socket_address;
    socket_address.sin_family      = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port        = htons(port);

    printf("Binding to port %d\n", port);

    int returnval;
    returnval = bind(listening_socket, (struct sockaddr *)&socket_address,
                     sizeof(socket_address));
    if (returnval == -1) {
        perror("bind");
        close(listening_socket);
        return -1;
    }
    returnval = listen(listening_socket, LISTEN_BACKLOG);
    return listening_socket;
}

void acceptConnections(int listening_socket) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    while (1) {
        pthread_t thread_id;
        int *client_fd_buf = malloc(sizeof(int));

        *client_fd_buf =
            accept(listening_socket, (struct sockaddr *)&client_address,
                   &client_address_len);
        if (*client_fd_buf < 0) {
            perror("accept");
            free(client_fd_buf);
            continue;
        }
        pthread_create(&thread_id, NULL,
                       (void *(*)(void *))handleConnectionThread,
                       client_fd_buf);
        pthread_detach(thread_id);
    }
}

void handleConnection(int a_client) {
    char read_buffer[BUFFER_SIZE];
    char write_buffer[LONGER_BUFFER_SIZE];
    int write_len = 0;
    while (1) {
        int bytes_read = read(a_client, read_buffer, sizeof(read_buffer));
        if (bytes_read <= 0) {
            if (bytes_read < 0)
                perror("read");
            else
                printf("Client disconnected.\n");
            break;
        }

        if (print_flag)
            printf("Read from client: %.*s\n", bytes_read, read_buffer);

        if (write_len + bytes_read >= LONGER_BUFFER_SIZE) {
            fprintf(stderr, "Line too long, dropping connection.\n");
            break;
        }

        memcpy(write_buffer + write_len, read_buffer, bytes_read);
        write_len += bytes_read;

        int start = 0;

        for (int buf_in = 0; buf_in < write_len; buf_in++) {
            if (write_buffer[buf_in] == '\n') {
                write(a_client, write_buffer + start, buf_in - start + 1);
                start = buf_in + 1;
            }
        }
        if (start < write_len) {
            memmove(write_buffer, write_buffer + start, write_len - start);
            write_len -= start;
        } else {
            write_len = 0;
        }
    }
    close(a_client);
}

void *handleConnectionThread(void *arg) {
    int client_fd = *((int *)arg);
    free(arg);
    handleConnection(client_fd);
    return NULL;
}
