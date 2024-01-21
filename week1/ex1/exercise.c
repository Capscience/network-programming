// #include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void elapsed_time(struct timeval start)
{
	// Get start time for elapsed
	struct timeval now;
	struct timezone tz;
	if (gettimeofday(&now, &tz) != 0) {
		printf("gettimeofday failed!");
		return;
	}
	time_t sec = now.tv_sec - start.tv_sec;
	suseconds_t usec = now.tv_usec - start.tv_usec;
	suseconds_t total = usec + 1000000 * sec;
	fprintf(stderr, "%ld microseconds\n", total);
}

void part_a()
{
	struct timeval start;
	struct timezone tz;
	gettimeofday(&start, &tz);

	pid_t pid = getpid();
	pid_t ppid = getppid();
	pid_t pgrp = getpgrp();
	uid_t uid = getuid();
	gid_t gid = getgid();

	elapsed_time(start);

	printf("%d\n", pid);
	printf("%d\n", ppid);
	printf("%d\n", pgrp);
	printf("%d\n", uid);
	printf("%d\n", gid);
}

void part_b(char *envp[])
{
	// Get start time for elapsed
	struct timeval start;
	struct timezone tz;
	gettimeofday(&start, &tz);

	// Loop through and print environment variables.
	for (char **env = envp; *env != 0; env++) {
		char *current = *env;
		printf("%s\n", *env);
	}

	elapsed_time(start);
}

int main(int argc, char *argv[], char *envp[])
{
	part_a();
	part_b(envp);
	return 0;
}
