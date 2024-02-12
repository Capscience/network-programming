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

void handler()
{
}
int reader(int fd_in, int fd_out, const struct sockaddr *server_addr,
	   int servlen)
{
	char *readbuf = malloc(BUF_SIZE);
	char *writebuf = malloc(BUF_SIZE + 1);
	int bytes_read;
	int w_index = 0;
	bool too_long = false;

	// Read a bufferfull from stdin
	while ((bytes_read = read(fd_in, readbuf, BUF_SIZE)) != -1) {
		// Loop over read bytes and write them into writebuf
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= BUF_SIZE) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = readbuf[i];
			if (readbuf[i] == *"\n") {
				if (!too_long) {
					if (sendto(fd_out, writebuf, w_index, 0,
						   server_addr,
						   servlen) != w_index) {
						perror("reader: sendto error");
						exit(EXIT_FAILURE);
					}
					w_index = recv(fd_out, writebuf,
						       w_index, 0);
					if (w_index < 0) {
						perror("reader: recv_from error");
						exit(EXIT_FAILURE);
					}
					write(STDOUT_FILENO, writebuf, w_index);
					w_index = recv(fd_out, writebuf,
						       w_index, 0);
					if (w_index < 0) {
						perror("reader: recv_from error");
						exit(EXIT_FAILURE);
					}
					write(STDOUT_FILENO, writebuf, w_index);
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

	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	if (inet_aton(addr, &server.sin_addr) == 0) {
		perror("Invalid ip-address");
		exit(EXIT_FAILURE);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	if ((connect(server_fd, (struct sockaddr *)&server, sizeof(server))) <
	    0) {
		perror("Connecting failed!");
		exit(1);
	}

	reader(fd_in, server_fd, (struct sockaddr *)&server, sizeof(server));
	close(server_fd);
	return EXIT_SUCCESS;
}
