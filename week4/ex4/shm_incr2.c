#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/* default permissions for new files */
#define LOCKFILE "/tmp/nwp_lockfile"
#define SHAREFILE "/tmp/nwp_sharefile"

void my_lock_init(char *pathname);
void my_lock_wait();
void my_lock_release();

int main(int argc, char **argv)
{
	int fd, i = 0, nloop, zero = 0;
	int *ptr;
	int tmp;
	pid_t pid;
	char *c, *buf;

	if (argc != 3) {
		perror("usage: incr2 <#loops> <char>");
		exit(1);
	}
	nloop = atoi(argv[1]);
	c = argv[2];

	/* 4open file, initialize to 0, map into memory */
	if ((fd = open(SHAREFILE, O_RDWR | O_CREAT, FILE_MODE)) < 0) {
		perror("open failed");
		exit(1);
	}
	if (write(fd, &zero, sizeof(int)) < 0) {
		perror("write failed");
		exit(1);
	}
	ptr = mmap(NULL, sizeof(int) + 1024 * sizeof(char),
		   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap failed");
		exit(1);
	}
	buf = (void *)ptr + sizeof(int);
	close(fd);

	/* 4create, initialize, and unlink file lock */
	my_lock_init(LOCKFILE);

	setbuf(stdout, NULL); /* stdout is unbuffered */
	pid = getpid();

	/* 4parent */
	while (i < nloop) {
		my_lock_wait();
		printf("%d\n", *ptr);
		if (*ptr == 0 || buf[(*ptr) - 1] != *c) {
			buf[(*ptr)++] = *c;
			i++;
		}
		my_lock_release();
		usleep(10);
	}
	my_lock_wait();
	write(STDOUT_FILENO, buf, *ptr + 1);
	my_lock_release();
	printf("%s\n", buf);
	unlink(LOCKFILE);
	unlink(SHAREFILE);
	exit(0);
}
