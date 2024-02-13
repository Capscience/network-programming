#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 100
#define PORT 50420

int reader(int fd_in, int sockfd)
{
	char *buf = malloc(BUF_SIZE);
	char *writebuf = malloc(BUF_SIZE);
	int bytes_read;
	int w_index = 0;
	bool too_long = false;

	while ((bytes_read = read(fd_in, buf, BUF_SIZE)) != -1) {
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= BUF_SIZE) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = buf[i];
			if (buf[i] == *"\n") {
				if (!too_long) {
					write(sockfd, writebuf, w_index);
					if (read(sockfd, writebuf, w_index) <
					    0) {
						perror("Read error");
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
	char *fqdn;

	if (argc == 3) {
		fqdn = argv[1];
		fd_in = open(argv[2], O_RDONLY);
	} else if (argc == 2) {
		fqdn = argv[1];
	} else {
		perror("Usage: ./client <fqdn> [inputfile]");
		exit(1);
	}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	struct hostent *hp = gethostbyname(fqdn);

	bcopy(hp->h_addr_list[0], &server.sin_addr, hp->h_length);
	server.sin_family = hp->h_addrtype;
	server.sin_port = htons(PORT);

	if ((connect(server_fd, (struct sockaddr *)&server, sizeof(server))) <
	    0) {
		perror("Connecting failed!");
		exit(1);
	}

	reader(fd_in, server_fd);
	close(server_fd);
	return EXIT_SUCCESS;
}
