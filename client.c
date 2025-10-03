#include "libtslog.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

static int sock;

void* recv_thread(void *arg) {
    (void)arg;
    char buf[512];
    while (1) {
        int r = recv(sock, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        printf("[Servidor] %s\n", buf);
    }
    return NULL;
}

int start_client(const char *host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return -1;
    }

    log_info("Conectado ao servidor %s:%d", host, port);

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);
    pthread_detach(tid);

    char buf[512];
    while (fgets(buf, sizeof(buf), stdin)) {
        send(sock, buf, strlen(buf), 0);
    }

    close(sock);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <ip> <porta>\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);

    log_init("logs/etapa2_client.log", LOG_INFO, 1);

    start_client(host, port);

    log_shutdown();
    return 0;
}

