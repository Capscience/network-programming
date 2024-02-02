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
	char *argv[3];
	argv[0] = path;
	argv[1] = arg;
	argv[2] = NULL;

	int ret = execv(path, argv);
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
		for (i = 1; i < 5; i++)
			printf("%d\n", i);
	} else {
		do_child(argv[1], argv[2]);
	}
	exit(0);
}
