#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void usage()
{
	char str[50];
	int n = sprintf(str, "Usage:\n\t./executable <path> [arg]\n");
	write(STDOUT_FILENO, str, n);
}

int do_child(char *path, char *arg)
{
	int ret = execv(path, &arg);
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		exit(1);
	}
	int i;
	pid_t pid = fork();
	if (pid < 0)
		exit(1);

	if (pid == 0) {
		for (i = 1; i < argc; i++)
			printf("%s\n", argv[i]);
	} else {
		do_child(argv[1], argv[2]);
	}
	exit(0);
}
