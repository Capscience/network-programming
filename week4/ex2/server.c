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

int doubler(int sockfd)
{
	char *buf = malloc(BUF_SIZE);
	char *writebuf = malloc(BUF_SIZE);
	int bytes_read;
	int w_index = 0;
	bool too_long = false;

	// Read from stdin (well actually a TCP socket)
	while ((bytes_read = read(sockfd, buf, BUF_SIZE)) > 0) {
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= BUF_SIZE) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = buf[i];
			if (buf[i] == *"\n") {
				if (!too_long) {
					write(sockfd, writebuf, w_index);
				}
				w_index = 0;
				too_long = false;
			}
		}
	}

	free(buf);
	free(writebuf);
	return -1;
}

int main(int argc, char *argv[])
{
	int server_fd, client_fd;
	struct sockaddr_in server, client;
	pid_t childpid;

	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = PF_INET;
	server.sin_port = htons(PORT);

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
		if ((childpid = fork()) < 0)
			perror("Fork failed!");
		else if (childpid == 0) {
			close(server_fd);
			doubler(client_fd);
			exit(0);
		}
		close(client_fd);
	}

	return EXIT_SUCCESS;
}
