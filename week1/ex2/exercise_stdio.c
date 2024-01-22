#include <stdio.h>
#include <stdlib.h>

int doubler(FILE *file_in)
{
	char *string = NULL;
	size_t n = 0;
	while (getline(&string, &n, file_in) != -1) {
		printf("%s", string);
		printf("%s", string);
	};
	free(string);
	return 1;
}

int main(int argc, char *argv[])
{
	FILE *file_in;
	if (argc == 2) {
		file_in = fopen(argv[1], "r");
	} else {
		file_in = stdin;
	}
	return doubler(file_in);
}
