#include "libtslog.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

static int sock;
static char username[32];

void* recv_thread(void *arg) {
    (void)arg;
    char buf[512];
    while (1) {
        int r = recv(sock, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        printf("%s", buf);
        fflush(stdout);
    }
    return NULL;
}

int start_client(const char *host, int port) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port) };
    inet_pton(AF_INET, host, &addr.sin_addr);
    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    printf("Digite seu nome: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    send(sock, username, strlen(username), 0);

    pthread_t tid;
    pthread_create(&tid, NULL, recv_thread, NULL);
    pthread_detach(tid);

    char msg[512];
    while (fgets(msg, sizeof(msg), stdin)) {
        if (strncmp(msg, "/exit", 5) == 0) break;
        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
    log_info("Cliente desconectado");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <ip> <porta>\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);

    log_init("logs/client.log", LOG_INFO, 1);
    start_client(host, port);
    log_shutdown();
    return 0;
}
