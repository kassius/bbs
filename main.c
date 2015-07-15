#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <iconv.h>

#include <libtelnet.h>

#include "config.h"
#include "main.h"

#include "input.c"
#include "draw.c"
#include "net.c"
#include "events.c"

static struct user_t users[MAX_USERS];

void _user_init(void *user_data)
{
	struct user_t *user = (struct user_t*)user_data;
	user->textmode = 0;
	//memset user buffer to 0
}

int main(int argc, char *argv[])
{
	int listen_sock;
	struct sockaddr_in addr;
	socklen_t addrlen;
	
	int rs;
	struct pollfd pfd[MAX_USERS + 1];

	char buffer[512];
	
	int i;

	memset(&pfd, 0, sizeof(pfd));
	memset(users, 0, sizeof(users));
	
	for(i=0; i<MAX_USERS; i++)
		{ users[i].sock = -1; }
	
	if((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cant open socket\n");
		exit(errno);
	}
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&rs, sizeof(rs));
	rs = 1;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(MY_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if( bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
	{
		perror("cant bind");
		exit(errno);
	}

	if( listen(listen_sock, 20) != 0 )
	{
		perror("cant listen");
		exit(errno);
	}

	pfd[MAX_USERS].fd = listen_sock;
	pfd[MAX_USERS].events = POLLIN;

	while(1)
	{
		for(i=0; i<MAX_USERS; i++)
		{
			if(users[i].sock != -1)
			{
				pfd[i].fd = users[i].sock;
				pfd[i].events = POLLIN;
			}
			else
			{
				pfd[i].fd = -1;
				pfd[i].events = 0;
			}	
		}

		rs = poll(pfd, MAX_USERS + 1, -1);
		if(rs == -1 && errno == EINTR)
		{
			printf("Poll failed\n");
			return 1;
		}

		if(pfd[MAX_USERS].revents & POLLIN)
		{
		
			addrlen = sizeof(addr);

			if((rs = accept(listen_sock, (struct sockaddr*)&addr, &addrlen)) == -1)
			{
				fprintf(stderr, "accept() failed %d.\n", errno);
				return 1;
			}

			printf("Connected client: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

			for(i=0; i<MAX_USERS; i++)
			{
				if(users[i].sock == -1)
					break;
					
				if(i == MAX_USERS)
				{
					printf("Connection reset.. too many users\n");
					//_send(rs, "Too many users.. please try again later\n\r");
					close(rs);
				}
			}
			
			users[i].sock = rs;
			users[i].telnet = telnet_init(telopts, _event_handler, 0, &users[i]);

			telnet_negotiate(users[i].telnet, TELNET_DO, TELNET_TELOPT_NAWS);
			telnet_negotiate(users[i].telnet, TELNET_WILL, TELNET_TELOPT_ECHO);
			telnet_negotiate(users[i].telnet, TELNET_WILL, TELNET_TELOPT_SGA);
			telnet_negotiate(users[i].telnet, TELNET_WONT, TELNET_TELOPT_LINEMODE);
		}

		for(i=0; i < MAX_USERS; i++)
		{
			if(users[i].sock == -1)
				continue;
				
			if(pfd[i].revents & POLLIN)
			{
				if( (rs = recv(users[i].sock, buffer, sizeof(buffer), 0)) > 0)
				{
					telnet_recv(users[i].telnet, buffer, sizeof(buffer));
				}
				else if(rs == 0)
				{
					printf("connection closed\n");
					close(users[i].sock);
					telnet_free(users[i].telnet);
					users[i].sock = -1;
					break;
				}
				else if(errno != EINTR)
				{
					fprintf(stderr, "recv() failed\n");
					exit(1);
				}
			}
		}
		//send(clientfd, "Hello, world!", sizeof("Hello, world!"), 0);

	}

	close(listen_sock);

	return 0;
}
