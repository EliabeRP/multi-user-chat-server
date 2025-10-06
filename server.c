#include "libtslog.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 50
#define BUF_SIZE 512

typedef struct {
    int sock;
    char name[32];
} client_t;

static client_t clients[MAX_CLIENTS];
static int nclients = 0;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const char *msg, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < nclients; i++) {
        if (clients[i].sock != sender_sock) {
            send(clients[i].sock, msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < nclients; i++) {
        if (clients[i].sock == sock) {
            clients[i] = clients[nclients - 1];
            nclients--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* client_thread(void *arg) {
    client_t cli = *(client_t*)arg;
    char buf[BUF_SIZE];
    log_info("Cliente conectado: %s (fd=%d)", cli.name, cli.sock);

    while (1) {
        int r = recv(cli.sock, buf, sizeof(buf)-1, 0);
        if (r <= 0) {
            log_info("Cliente desconectou: %s", cli.name);
            close(cli.sock);
            remove_client(cli.sock);
            pthread_exit(NULL);
        }
        buf[r] = '\0';
        char msg[BUF_SIZE + 64];
        snprintf(msg, sizeof(msg), "%s: %s", cli.name, buf);
        log_info("%s", msg);
        broadcast(msg, cli.sock);
    }
}

int start_server(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr.s_addr = INADDR_ANY };
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    log_info("Servidor iniciado na porta %d", port);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        pthread_mutex_lock(&clients_mutex);
        if (nclients >= MAX_CLIENTS) {
            log_warn("Limite de clientes atingido");
            close(client_fd);
        } else {
            client_t cli = { .sock = client_fd };
            recv(client_fd, cli.name, sizeof(cli.name)-1, 0);
            cli.name[strcspn(cli.name, "\n")] = 0;

            clients[nclients++] = cli;
            pthread_t tid;
            pthread_create(&tid, NULL, client_thread, &clients[nclients-1]);
            pthread_detach(tid);
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <porta>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    log_init("logs/server.log", LOG_INFO, 1);
    start_server(port);
    log_shutdown();
    return 0;
}
