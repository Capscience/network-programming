#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void myhandler(int sig)
{
	write(STDERR_FILENO, "\n\nSIGINT received\n", 18);
	exit(1);
}

void copy(char *path)
{
	int fd_in = open(path, O_RDONLY);
	int bufsize = 100;
	char *buf = malloc(bufsize * sizeof(char));

	ssize_t bytes_read = 0;
	while ((bytes_read = read(fd_in, buf, bufsize))) {
		write(STDOUT_FILENO, buf, bufsize);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, myhandler);
	if (argc != 2) {
		write(STDERR_FILENO,
		      "Invalid argument count, expected filename.", 42);
		exit(1);
	}

	copy(argv[1]);

	return 0;
}
