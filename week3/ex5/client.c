#include <bits/types/siginfo_t.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUF_SIZE 100
#define PORT 50420

int main(int argc, char *argv[])
{
	int server_fd;
	struct sockaddr_in server;
	char *addr;

	if (argc != 4) {
		perror("Usage: ./client <ipv4-addr> <#bytes> <bufsize>");
		exit(1);
	}
	unsigned long bytes = strtoul(argv[2], NULL, 10);
	int bufsize = atoi(argv[3]);

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

	char *buf = malloc(bufsize * sizeof(char));
	memset(buf, *"a", bufsize * sizeof(char));
	int iterations = bytes / bufsize;
	for (int i = 0; i < iterations; i++) {
		write(server_fd, buf, bufsize);
	}
	write(server_fd, buf, bytes % bufsize);
	close(server_fd);
	return EXIT_SUCCESS;
}
