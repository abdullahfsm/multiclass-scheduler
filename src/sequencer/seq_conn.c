#include "seq_conn.h"

bool init_UDP_socket(int *fd, int port, bool bindto, bool broadcast)
{

	int sockfd;
	int sock_opt=1;
    struct sockaddr_in src_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("socket");
        return false;
	}

	if(broadcast){
		if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &sock_opt,
	        sizeof sock_opt) == -1) {
	        perror("setsockopt (SO_BROADCAST)");
	        return false;
    	}
	}

    if(bindto){
    	memset(&src_addr, 0, sizeof(src_addr));
	    src_addr.sin_family = AF_INET;
	    src_addr.sin_addr.s_addr = INADDR_ANY;
	    if(broadcast)
	    	src_addr.sin_port = htons(BROADCAST);
	    else
	    	src_addr.sin_port = htons(port);

	    if (bind(sockfd,(struct sockaddr *)&src_addr,sizeof(struct sockaddr)) < 0){
        	perror("Error: bind");
            return false;
	    }
    }

    *fd = sockfd;
    return true;
}

bool send_UDP_seq_msg(char *ip, int port, int fd, struct seq_msg *smsg)
{

	struct sockaddr_in dst_addr;
	int numbytes;
	char buf[SEQ_MSG_SIZE] = {0};

	memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr = inet_addr(ip);
    dst_addr.sin_port = htons(port);

    seq_msg2buf(smsg,buf);

    if ((numbytes=sendto(fd, buf, SEQ_MSG_SIZE, 0, (struct sockaddr *)&dst_addr, sizeof dst_addr)) == -1) {
        perror("sendto");
        return false;
    }

    return true;
}

bool recv_UDP_seq_msg(int fd, struct seq_msg *smsg)
{

	struct sockaddr_in dst_addr;
	socklen_t addr_len;
	addr_len = sizeof dst_addr;
	int numbytes;
	
	char buf[SEQ_MSG_SIZE] = {0};

	if ((numbytes = recvfrom(fd, buf, SEQ_MSG_SIZE, 0,
		(struct sockaddr *)&dst_addr, &addr_len)) == -1) {
		perror("recvfrom");
		return false;
	}

	buf2seq_msg(buf,smsg);

	#ifdef DEBUG
	printf("numbytes: %d\n", numbytes);
	#endif

	return true;
}

bool send_seq_msg(int fd, struct seq_msg *smsg)
{
    int count = SEQ_MSG_SIZE;
    char buf[SEQ_MSG_SIZE] = {0};
    seq_msg2buf(smsg,buf);
    unsigned int bytes_total_write = 0; /* total number of bytes that have been written */
    unsigned int bytes_to_write = 0;    /* maximum number of bytes to write in next send() call */
    char *cur_buf = NULL;   /* current location */
    int n;  /* number of bytes read in current read() call */
    
    while (count > 0)
    {
        bytes_to_write = count;
        cur_buf = buf + bytes_total_write;
        n = write(fd, cur_buf, bytes_to_write);

        if (n <= 0)
        {
            break;
        }
        else
        {
            bytes_total_write += n;
            count -= n;
        }
    }

    if(bytes_total_write == SEQ_MSG_SIZE){
        return true;
    }
    return false;
}

bool recv_seq_msg(int fd, struct seq_msg *smsg)
{
    unsigned int bytes_total_read = 0;  /* total number of bytes that have been read */
    unsigned int bytes_to_read = 0; /* maximum number of bytes to read in next read() call */
    char *cur_buf = NULL;   /* current location */
    int n;  /* number of bytes read in current read() call */
    char buf[SEQ_MSG_SIZE] = {0};
    int count = SEQ_MSG_SIZE;

    while (count > 0)
    {
        bytes_to_read = count;
        cur_buf = buf + bytes_total_read;
        n = read(fd, cur_buf, bytes_to_read);

        if (n <= 0)
        {
            break;
        }
        else
        {
            bytes_total_read += n;
            count -= n;
        }
    }

	buf2seq_msg(buf, smsg);

    if(bytes_total_read == SEQ_MSG_SIZE){
        return true;
    }
    return false;
}


void seq_msg2buf(struct seq_msg *smsg,char *buf){
	/* constructing a message */
    memcpy(buf + offsetof(struct seq_msg, seq), &(smsg->seq), sizeof(smsg->seq));
    memcpy(buf + offsetof(struct seq_msg, fc), &(smsg->fc), sizeof(smsg->fc));
    memcpy(buf + offsetof(struct seq_msg, active), &(smsg->active), sizeof(smsg->active));
}

void buf2seq_msg(char *buf, struct seq_msg *smsg){
	/* copying content of buf into smsg */
	memcpy(&(smsg->seq), buf + offsetof(struct seq_msg, seq), sizeof(smsg->seq));
    memcpy(&(smsg->fc), buf + offsetof(struct seq_msg, fc), sizeof(smsg->fc));
    memcpy(&(smsg->active), buf + offsetof(struct seq_msg, active), sizeof(smsg->active));
}