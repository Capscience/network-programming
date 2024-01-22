#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

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

void sigint_handler(int signo)
{
	switch (signo) {
	case SIGINT:
		write(STDOUT_FILENO, "\nSIGINT\n", 8);
		_exit(1);
	}
}

int main(int argc, char *argv[])
{
	struct sigaction act;

	act.sa_handler = sigint_handler;
	sigaction(SIGINT, &act, NULL);
	if (argc != 2) {
		write(STDERR_FILENO,
		      "Invalid argument count, expected filename.", 42);
		exit(1);
	}

	copy(argv[1]);

	return 0;
}
