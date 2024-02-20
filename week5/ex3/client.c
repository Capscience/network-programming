#include <bits/types/siginfo_t.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 100
#define PORT 50420
#define MAX_RETRY 5

int reader(int fd_in, int fd_out)
{
	char *buf = malloc(BUF_SIZE);
	char *writebuf = malloc(BUF_SIZE);
	int bytes_read;
	int bytes_written = 1;
	int retry = 0;
	int w_index = 0;
	bool too_long = false;

	// Read from stdin
	while ((bytes_read = read(fd_in, buf, BUF_SIZE)) >= 0) {
		if (bytes_written == 0 && retry < MAX_RETRY) {
			retry++;
			sleep(1);
			continue;
		}
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= BUF_SIZE) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = buf[i];
			if (buf[i] == *"\n") {
				if (!too_long) {
					write(STDOUT_FILENO, "before\n", 7);
					bytes_written = write(fd_out, writebuf,
							      w_index);
					write(STDOUT_FILENO, "write\n", 6);
					if (bytes_written < 0) {
						perror("write error");
						exit(1);
					} else if (bytes_written == 0) {
						break;
					}
				}
				w_index = 0;
				too_long = false;
			}
		}
	}
	printf("%d\n", bytes_read);
	fflush(stdout);
	perror("reader");
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

	if (inet_aton(addr, &server.sin_addr) == 0) {
		perror("Invalid ip-address");
		exit(1);
	}
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT);

	if ((connect(server_fd, (struct sockaddr *)&server, sizeof(server))) <
	    0) {
		perror("Connecting failed!");
		exit(1);
	}
	// shutdown(fd_in, SHUT_RD);

	reader(fd_in, server_fd);
	close(server_fd);
	return EXIT_SUCCESS;
}
