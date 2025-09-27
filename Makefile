CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread

BIN = log_sim

all: $(BIN)

$(BIN): libtslog.c tests/log_sim.c include/libtslog.h
	$(CC) $(CFLAGS) libtslog.c tests/log_sim.c -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) logs/*.log

.PHONY: all run clean
