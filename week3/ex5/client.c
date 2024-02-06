#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
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
		printf("Wrote %lu bytes in %.5f seconds\t\t", bytes, secs);
	} else {
		printf("Wrote %.2f %sB in %.5f seconds\t\t", bytes_compact,
		       bytes_letter, secs);
	}

	if (bps_letter == NULL) {
		printf("%.0f B/s\n", bps);
	} else {
		printf("%.2f %sB/s\n", bps_compact, bps_letter);
	}
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	int server_fd;
	struct sockaddr_in server;
	char *addr;
	struct timeval start, end;
	struct timezone tz;

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

	gettimeofday(&start, &tz);
	if ((connect(server_fd, (struct sockaddr *)&server, sizeof(server))) <
	    0) {
		perror("Connecting failed!");
		exit(1);
	}
	gettimeofday(&end, &tz);
	time_t sec = end.tv_sec - start.tv_sec;
	suseconds_t usec = end.tv_usec - start.tv_usec + 1000000 * sec;
	float secs = (float)usec / 1000000;
	printf("Connection established in %.5f seconds\n", secs);

	char *buf = malloc(bufsize * sizeof(char));
	memset(buf, *"a", bufsize * sizeof(char));
	int iterations = bytes / bufsize;
	gettimeofday(&start, &tz);
	for (int i = 0; i < iterations; i++) {
		write(server_fd, buf, bufsize);
	}
	write(server_fd, buf, bytes % bufsize);
	gettimeofday(&end, &tz);
	print_stats(start, end, bytes);
	close(server_fd);
	return EXIT_SUCCESS;
}
