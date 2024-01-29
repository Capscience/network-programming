#include <stdio.h>
int main(int argc, char *argv[])
{
	FILE *fh = fopen("/tmp/test.txt", "a");
	fprintf(fh, "Test successful");
	fclose(fh);
	return 0;
}
