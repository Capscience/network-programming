#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

const int MAX_LEN = 100;

int reader(int fd_in, int fd_out)
{
	char *buf = malloc(100 * sizeof(char));
	int bytes_read;
	bool line_ended = true;

	// Read line from fd_in
	while ((bytes_read = read(fd_in, buf, MAX_LEN))) {
		// This if ignores too long lines
		if (buf[bytes_read - 1] == *"\n") {
			if (line_ended)
				// Write to fd_out if line not too long
				write(fd_out, buf, bytes_read);
			line_ended = true;
		} else {
			line_ended = false;
		}
	}

	free(buf);
	return -1;
}

int doubler(int fd_in, int fd_out)
{
	char *buf = malloc(100 * sizeof(char));
	int bytes_read;
	while ((bytes_read = read(fd_in, buf, MAX_LEN)) != -1) {
		write(fd_out, buf, bytes_read);
		write(fd_out, buf, bytes_read);
	}
	free(buf);
	return -1;
}

int main(int argc, char *argv[])
{
	int pipe1[2];
	if (pipe(pipe1) < 0) {
		perror("Pipe creation failed.");
		exit(EXIT_FAILURE);
	}
	pid_t pid = fork();
	if (pid < 0) {
		perror("Fork failed.");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		// Parent
		close(pipe1[0]);
		reader(STDIN_FILENO, pipe1[1]);
	} else {
		// Child
		close(pipe1[1]);
		doubler(pipe1[0], STDOUT_FILENO);
	}
	return EXIT_SUCCESS;
}
