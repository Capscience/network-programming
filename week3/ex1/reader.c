#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

#define FIFO_R_TO_C "/tmp/nwp_r_to_c"

#define MAX_LEN 100

int reader(int fd_in, int fd_out)
{
	char *buf = malloc(MAX_LEN);
	char *writebuf = malloc(MAX_LEN);
	int bytes_read;
	int w_index = 0;
	bool too_long = false;

	// Read from stdin
	while ((bytes_read = read(fd_in, buf, MAX_LEN)) != -1) {
		for (int i = 0; i < bytes_read; i++) {
			if (w_index >= 100) {
				too_long = true;
			}
			if (!too_long)
				writebuf[w_index++] = buf[i];
			if (buf[i] == *"\n") {
				if (!too_long) {
					write(fd_out, writebuf, w_index);
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
	int fd_in = STDIN_FILENO;
	if (argc == 2) {
		fd_in = open(argv[1], O_RDONLY);
	}
	// Create and/or open fifos
	int r_to_c_fd, c_to_d_fd;
	if ((mknod(FIFO_R_TO_C, S_IFIFO | 0666, 0) < 0) && (errno != EEXIST)) {
		write(STDERR_FILENO, "FIFO_R_TO_C creation failed\n", 24);
		exit(EXIT_FAILURE);
	}
	if ((r_to_c_fd = open(FIFO_R_TO_C, O_WRONLY)) < 0) {
		write(STDERR_FILENO, "FIFO_R_TO_C opening failed\n", 23);
		exit(EXIT_FAILURE);
	}

	reader(fd_in, r_to_c_fd);

	return EXIT_SUCCESS;
}
