#include <time.h>
#include <sys/time.h>
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

void print_stats(struct timeval start, struct timeval end, unsigned long bytes)
{
	char *bytes_letter = NULL;
	char *bps_letter = NULL;
	float bytes_compact, bps_compact;
	time_t sec = end.tv_sec - start.tv_sec;
	suseconds_t usec = end.tv_usec - start.tv_usec + 1000000 * sec;
	float secs = (float)usec / 1000000;
	float bps = bytes / secs;

	if (bytes >= 1000000000) {
		bytes_letter = "G";
		bytes_compact = (float)bytes / 1000000000;
	} else if (bytes >= 1000000) {
		bytes_letter = "M";
		bytes_compact = (float)bytes / 1000000;
	} else if (bytes >= 1000) {
		bytes_letter = "k";
		bytes_compact = (float)bytes / 1000;
	} else {
		bytes_compact = (float)bytes;
	}

	if (bps >= 1000000000) {
		bps_letter = "G";
		bps_compact = (float)bps / 1000000000;
	} else if (bps >= 1000000) {
		bps_letter = "M";
		bps_compact = (float)bps / 1000000;
	} else if (bps >= 1000) {
		bps_letter = "k";
		bps_compact = (float)bps / 1000;
	} else {
		bps_compact = (float)bps;
	}

	if (bytes_letter == NULL) {
		printf("Read %lu bytes in %.5f seconds\t\t", bytes, secs);
	} else {
		printf("Read %.2f %sB in %.5f seconds\t\t", bytes_compact,
		       bytes_letter, secs);
	}

	if (bps_letter == NULL) {
		printf("%.0f B/s\n", bps);
	} else {
		printf("%.2f %sB/s\n", bps_compact, bps_letter);
	}
	fflush(stdout);
}

int ignorer(int fd_in, int buf_size)
{
	bool start_set = false;
	struct timeval start, end;
	struct timezone tz;

	unsigned long count = 0;
	char *buf = malloc(buf_size);
	int bytes_read;
	while ((bytes_read = read(fd_in, buf, BUF_SIZE)) > 0) {
		if (!start_set) {
			gettimeofday(&start, &tz);
			start_set = true;
		}
		count += bytes_read;
	}

	gettimeofday(&end, &tz);
	print_stats(start, end, count);
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
