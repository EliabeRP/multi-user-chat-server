#include "libtslog.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int tid;
    int nmsgs;
} thread_arg_t;

void* worker(void *arg) {
    thread_arg_t *t = (thread_arg_t*) arg;
    for (int i = 0; i < t->nmsgs; i++) {
        log_info("thread-%d msg-%d", t->tid, i);
        if (i % 8 == 0) sched_yield();
    }
    return NULL;
}

int main(int argc, char **argv) {
    int nthreads = 8;
    int nmsgs = 100;

    if (argc >= 2) nthreads = atoi(argv[1]);
    if (argc >= 3) nmsgs = atoi(argv[2]);

    system("mkdir -p logs");
    log_init("logs/etapa1_c.log", LOG_DEBUG, 1);

    pthread_t *ths = malloc(nthreads * sizeof(pthread_t));
    thread_arg_t *args = malloc(nthreads * sizeof(thread_arg_t));

    for (int i = 0; i < nthreads; i++) {
        args[i].tid = i;
        args[i].nmsgs = nmsgs;
        pthread_create(&ths[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < nthreads; i++) pthread_join(ths[i], NULL);

    log_shutdown();
    free(ths);
    free(args);

    printf("Finished logging. Check logs/etapa1_c.log\n");
    return 0;
}
