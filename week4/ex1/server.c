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

int doubler(int sockfd, struct sockaddr *client, socklen_t clilen)
{
	char *buf = malloc(BUF_SIZE);
	int bytes_read;
	socklen_t len;

	for (;;) {
		len = clilen;
		bytes_read = recvfrom(sockfd, buf, BUF_SIZE, 0, client, &len);
		if (bytes_read < 0) {
			perror("recvfrom error");
			exit(EXIT_FAILURE);
		}
		if (sendto(sockfd, buf, bytes_read, 0, client, len) !=
		    bytes_read) {
			perror("sendto error");
			exit(EXIT_FAILURE);
		}
		if (sendto(sockfd, buf, bytes_read, 0, client, len) !=
		    bytes_read) {
			perror("sendto error");
			exit(EXIT_FAILURE);
		}
	}

	free(buf);
	return -1;
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in server, client;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed!");
		exit(1);
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed!");
		exit(1);
	}
	doubler(sockfd, (struct sockaddr *)&client, sizeof(client));
	return EXIT_SUCCESS;
}
