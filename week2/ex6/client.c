#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

#define FIFO_REQ "/tmp/nwp_req"
#define FIFO_ANS "/tmp/nwp_ans"

const int MAX_LEN = 100;

int reader(int fd_in, int fd_out, int reqfd, int ansfd)
{
	char *buf = malloc(100 * sizeof(char));
	int bytes_read;
	bool line_ended = true;

	// Read from stdin
	while ((bytes_read = read(fd_in, buf, MAX_LEN)) != -1) {
		// Ignore too long lines
		if (buf[bytes_read - 1] == *"\n") {
			if (line_ended) {
				// Write to request fifo
				write(reqfd, buf, bytes_read);
				// Read from answer fifo
				bytes_read = read(ansfd, buf, bytes_read);
				// Write to stdout
				write(fd_out, buf, bytes_read);
			}
			line_ended = true;
		} else {
			line_ended = false;
		}
	}

	free(buf);
	return -1;
}

int main(int argc, char *argv[])
{
	// Create and/or open fifos
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
	if ((reqfd = open(FIFO_REQ, O_WRONLY)) < 0) {
		write(STDERR_FILENO, "FIFO_REQ opening failed\n", 23);
		exit(EXIT_FAILURE);
	}
	if ((ansfd = open(FIFO_ANS, O_RDONLY)) < 0) {
		write(STDERR_FILENO, "FIFO_ANS opening failed\n", 23);
		exit(EXIT_FAILURE);
	}

	reader(STDIN_FILENO, STDOUT_FILENO, reqfd, ansfd);

	return EXIT_SUCCESS;
}
