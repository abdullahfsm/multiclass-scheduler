CC = gcc
CFLAGS = -g -c -Wall -pthread -lm -lrt -I/usr/include/mysql
LDFLAGS = -pthread -lm -lrt

TARGETS = client server sequencer
CLIENT_OBJS = queue.o common.o cdf.o conn.o multiclass_scheduler.o seq_conn.o seqmanager.o client.o
SERVER_OBJS = common.o server.o
SEQ_OBJS = common.o seq_conn.o sequencer.o
BIN_DIR = bin
RESULT_DIR = results
CLIENT_DIR = src/client
COMMON_DIR = src/common
SERVER_DIR = src/server
SEQ_DIR = src/sequencer
SCHED_DIR = src/scheduler

all: $(TARGETS) move

move:
	mkdir -p $(RESULT_DIR)
	mkdir -p $(BIN_DIR)
	mv *.o $(TARGETS) $(BIN_DIR)

client: $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o client $(LDFLAGS)

server: $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o server $(LDFLAGS)

sequencer: $(SEQ_OBJS)
	$(CC) $(SEQ_OBJS) -o sequencer $(LDFLAGS)

%.o: $(CLIENT_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: $(SERVER_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: $(SEQ_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: $(SCHED_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: $(COMMON_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)/*