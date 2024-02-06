#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUF_SIZE 100
#define PORT 50420

int ignorer(int fd_in, int buf_size)
{
	unsigned long count = 0;
	char *buf = malloc(buf_size);
	int bytes_read;
	while ((bytes_read = read(fd_in, buf, BUF_SIZE)) > 0) {
		count += bytes_read;
	}
	printf("Read %lu bytes\n", count);
	fflush(stdout);
	free(buf);
	return -1;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		perror("Usage: ./server <bufsize>");
		exit(1);
	}
	int buf_size = atoi(argv[1]);

	int server_fd, client_fd;
	struct sockaddr_in server, client;

	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_family = PF_INET;
	server.sin_port = PORT;

	if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed!");
		exit(1);
	}

	if (listen(server_fd, 8) < 0) {
		perror("Bind failed!");
		exit(1);
	}

	for (;;) {
		socklen_t client_len = sizeof(client);
		if ((client_fd = accept(server_fd, (struct sockaddr *)&client,
					&client_len)) < 0) {
			perror("Accept failed!");
			exit(1);
		}

		ignorer(client_fd, buf_size);
	}

	return EXIT_SUCCESS;
}
