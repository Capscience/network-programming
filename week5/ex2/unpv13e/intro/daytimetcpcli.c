#include "unp.h"

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;

	if (argc != 3)
		err_quit("usage: a.out <IPaddress> <#connections>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(50420); /* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	for (int i = 0; i < atoi(argv[2]); i++) {
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			err_sys("socket error");

		if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
			err_sys("connect error");

		while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
			recvline[n] = 0; /* null terminate */
			if (fputs(recvline, stdout) == EOF)
				err_sys("fputs error");
		}
		if (n < 0)
			err_sys("read error");
		close(sockfd);
	}

	exit(0);
}
