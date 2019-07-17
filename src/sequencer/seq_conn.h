#ifndef SEQCON_H
#define SEQCON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stddef.h>

#define BROADCAST 6666	// BROADCAST port clients will bind to
#define TG_SEQ_PORT 6001

struct seq_msg
{
	unsigned int seq;
	unsigned int fc;
	unsigned int active;
};

#define SEQ_MSG_SIZE (sizeof(struct seq_msg))



bool init_UDP_socket(int *fd, int port, bool bindto, bool broadcast);

bool send_UDP_seq_msg(char *ip, int port, int fd, struct seq_msg *smsg);
bool recv_UDP_seq_msg(int fd, struct seq_msg *smsg);

bool send_seq_msg(int fd, struct seq_msg *smsg);
bool recv_seq_msg(int fd, struct seq_msg *smsg);

void seq_msg2buf(struct seq_msg *smsg,char *buf);
void buf2seq_msg(char *buf, struct seq_msg *smsg);

#endif