#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

#define FIFO_REQ "/tmp/nwp_req"
#define FIFO_ANS "/tmp/nwp_ans"

const int MAX_LEN = 100;

int lower_server(int fd_in, int fd_out)
{
	char *buf = malloc(100 * sizeof(char));
	int bytes_read;
	// Read from request fifo
	while ((bytes_read = read(fd_in, buf, MAX_LEN)) != -1) {
		for (int i = 0; i < bytes_read; i++) {
			buf[i] = tolower(buf[i]);
		}
		// Write to answer fifo
		write(fd_out, buf, bytes_read);
	}
	free(buf);
	return -1;
}

int main(int argc, char *argv[])
{
	// Create fifos
	int reqfd, ansfd;
	if ((mknod(FIFO_REQ, S_IFIFO | 0666, 0) < 0) && (errno != EEXIST)) {
		write(STDERR_FILENO, "FIFO_REQ creation failed\n", 24);
		exit(EXIT_FAILURE);
	}
	if ((mknod(FIFO_ANS, S_IFIFO | 0666, 0) < 0) && (errno != EEXIST)) {
		write(STDERR_FILENO, "FIFO_ANS creation failed\n", 24);
		unlink(FIFO_REQ);
		exit(EXIT_FAILURE);
	}
	if ((reqfd = open(FIFO_REQ, O_RDONLY)) < 0) {
		write(STDERR_FILENO, "FIFO_REQ opening failed\n", 23);
		exit(EXIT_FAILURE);
	}
	if ((ansfd = open(FIFO_ANS, O_WRONLY)) < 0) {
		write(STDERR_FILENO, "FIFO_ANS opening failed\n", 23);
		exit(EXIT_FAILURE);
	}

	lower_server(reqfd, ansfd);
	return EXIT_SUCCESS;
}
