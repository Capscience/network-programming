#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void usage()
{
	char str[50];
	int n = sprintf(str, "Usage: ./executable [processes (5-50)]\n");
	write(STDOUT_FILENO, str, n);
}

int main(int argc, char *argv[])
{
	int i;
	int counter = 0;
	int children = 4;
	if (argc > 1) {
		int arg1 = atoi(argv[1]);
		if (arg1 < 5 || arg1 > 50 || argc > 2) {
			usage();
			exit(1);
		}
		children = arg1;
	}
	pid_t pid = 0;
	while (counter < children) {
		if (pid == 0)
			pid = fork();
		// Fork failed, exit with error
		if (pid < 0)
			exit(1);
		counter++;
	}

	for (i = 1; i <= 5; i++) {
		char str[50];
		int n = sprintf(str, "%d: i = %d \n", pid, i);
		write(STDOUT_FILENO, str, n);
	}
	exit(0);
}
