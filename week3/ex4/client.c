#include <bits/types/siginfo_t.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUF_SIZE 100
#define PORT 50420

void handler()
{
}
int reader(int fd_in, int fd_out)
{
	char *buf = malloc(BUF_SIZE);
	char *writebuf = malloc(BUF_SIZE);
	int bytes_read;
	int w_index = 0;
	bool too_long = false;

	// Read from stdin
	while ((bytes_read = read(fd_in, buf, BUF_SIZE)) != -1) {
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= 100) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = buf[i];
			if (buf[i] == *"\n") {
				if (!too_long) {
					write(fd_out, writebuf, w_index);
				}
				w_index = 0;
				too_long = false;
			}
		}
	}
	return -1;
}

int main(int argc, char *argv[])
{
	int server_fd;
	struct sockaddr_in server;
	int fd_in = STDIN_FILENO;
	char *addr;

	if (argc == 3) {
		addr = argv[1];
		fd_in = open(argv[2], O_RDONLY);
	} else if (argc == 2) {
		addr = argv[1];
	} else {
		perror("Usage: ./client <ipv4-addr> [inputfile]");
		exit(1);
	}

	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = PF_INET;
	server.sin_port = PORT;

	if ((connect(server_fd, (struct sockaddr *)&server, sizeof(server))) <
	    0) {
		perror("Connecting failed!");
		exit(1);
	}

	reader(fd_in, server_fd);
	close(server_fd);
	return EXIT_SUCCESS;
}
