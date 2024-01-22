#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int readline(int fd_in, char *buf, ssize_t max)
{
	int pos = 0;
	// Read one byte at a time
	while (pos < max && read(fd_in, &buf[pos], 1)) {
		// Stop when \n or \0 found
		if (buf[pos] == *"\n" || buf[pos] == *"\0")
			return pos + 1;
		pos++;
	}
	// Failure if buffer not large enough or file empty
	return -1;
}

int doubler(int fd_in, int fd_out)
{
	char *buf = malloc(100 * sizeof(char));
	size_t n = 100;
	ssize_t bytes_read = 0;
	while ((bytes_read = readline(fd_in, buf, n)) != -1) {
		write(fd_out, buf, bytes_read);
		write(fd_out, buf, bytes_read);
	};
	free(buf);
	return 1;
}

int main(int argc, char *argv[])
{
	int fd_in;
	if (argc == 2) {
		fd_in = open(argv[1], O_RDONLY);
	} else {
		fd_in = STDIN_FILENO;
	}
	return doubler(fd_in, STDOUT_FILENO);
}
