#include "libtslog.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100

static int clients[MAX_CLIENTS];
static int nclients = 0;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const char *msg, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < nclients; i++) {
        if (clients[i] != sender_fd) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* client_handler(void *arg) {
    int sock = *(int*)arg;
    char buf[512];
    snprintf(buf, sizeof(buf), "Cliente %d conectado\n", sock);
    log_info("%s", buf);

    while (1) {
        int r = recv(sock, buf, sizeof(buf)-1, 0);
        if (r <= 0) {
            log_info("Cliente %d desconectou", sock);
            close(sock);

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < nclients; i++) {
                if (clients[i] == sock) {
                    clients[i] = clients[nclients-1];
                    nclients--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            break;
        }
        buf[r] = '\0';
        log_info("Recebido de %d: %s", sock, buf);
        broadcast(buf, sock);
    }
    return NULL;
}

int start_server(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return -1;
    }

    log_info("Servidor iniciado na porta %d", port);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        pthread_mutex_lock(&clients_mutex);
        if (nclients < MAX_CLIENTS) {
            clients[nclients++] = client_fd;
            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, &clients[nclients-1]);
            pthread_detach(tid);
        } else {
            close(client_fd);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_fd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <porta>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    log_init("logs/etapa2_server.log", LOG_INFO, 1);

    start_server(port);

    log_shutdown();
    return 0;
}