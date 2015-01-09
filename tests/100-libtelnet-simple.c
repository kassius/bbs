#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>

#include <libtelnet.h>

#define MY_PORT 9999

static const telnet_telopt_t telopts[] =
{
	{ TELNET_TELOPT_COMPRESS2,	TELNET_WILL,	TELNET_DONT },
	{ -1, 0, 0 }
}

struct user_t
{
	int sock;
	telnet_t *telnet;
}

static struct user_t user;

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in self;
	telnet_t *telnet;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cant open socket\n");
		exit(errno);
	}

	bzero(&self, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	if( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("cant bind");
		exit(errno);
	}

	if( listen(sockfd, 20) != 0 )
	{
		perror("cant listen");
		exit(errno);
	}

	while(1)
	{
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);

		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		printf("Connected client: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		telnet_init(telopts, _event_handler, 0, &user);
		//send(clientfd, "Hello, world!", sizeof("Hello, world!"), 0);

		close(clientfd);
	}

	close(sockfd);

	return 0;
}
