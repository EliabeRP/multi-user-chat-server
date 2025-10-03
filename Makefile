CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread

BIN_SERVER = server
BIN_CLIENT = client
BIN_TEST   = log_sim

all: $(BIN_SERVER) $(BIN_CLIENT)

server: server.c libtslog.c include/libtslog.h include/server.h
	$(CC) $(CFLAGS) server.c libtslog.c -o $(BIN_SERVER)

client: client.c libtslog.c include/libtslog.h include/client.h
	$(CC) $(CFLAGS) client.c libtslog.c -o $(BIN_CLIENT)

log_sim: tests/log_sim.c libtslog.c include/libtslog.h
	$(CC) $(CFLAGS) tests/log_sim.c libtslog.c -o $(BIN_TEST)

run-server: server
	./server

run-client: client
	./client 127.0.0.1 8080

clean:
	rm -f $(BIN_SERVER) $(BIN_CLIENT) $(BIN_TEST) logs/*.log

.PHONY: all clean run-server run-client
