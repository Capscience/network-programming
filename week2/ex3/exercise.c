#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: ./executable <file path>");
		return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_RDWR);

	struct stat statbuf;
	if (fstat(fd, &statbuf) != -1) {
		printf("Owner uid: %d\n", statbuf.st_uid);
		printf("File size: %ld bytes\n", statbuf.st_size);
	} else {
		printf("fstat failed\n");
		printf("errno: %d\n", errno);
	}

	return EXIT_SUCCESS;
}
