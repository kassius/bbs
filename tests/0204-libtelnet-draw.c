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

#define MY_PORT 9999

static const telnet_telopt_t telopts[] =
{
    { TELNET_TELOPT_BINARY,    	TELNET_WILL,	 TELNET_DO },
    { TELNET_TELOPT_ECHO,    	TELNET_WILL,	 TELNET_DO },
	{ TELNET_TELOPT_NAWS,		TELNET_WILL,	TELNET_DO },
	//{ TELNET_TELOPT_COMPRESS2,	TELNET_WILL,	TELNET_DONT },
	//{ TELNET_TELOPT_XASCII, 	TELNET_WILL, 	TELNET_DO },
	{ -1, 0, 0 }
};

struct user_t
{
	int sock;
	telnet_t *telnet;

	int width, height;
};

static struct user_t user;

static void _send(int sock, const char *buffer, unsigned int size)
{
	int rs;
	unsigned int size_counter;
	
	const char *ptr;
	
	ptr = buffer;
	size_counter = size;

	if(sock == -1) return;

	while(size_counter > 0)
	{
		if((rs = send(sock, ptr, size_counter, 0)) == -1)
		{
			if(errno != EINTR && errno != ECONNRESET)
			{
				fprintf(stderr, "send() error\n");
				exit(1);
			}
			else return; 
		}
		else if(rs == 0)
		{
			fprintf(stderr, "send() returned 0\n");
			exit(1);
		}

		ptr += rs;
		size_counter -= rs;
	}
}

static int _draw(int width, int height, telnet_t *telnet)
{
	int i;

	unsigned char tl,hl,tr,vl,bl,br;

	char *buffer, *buffer2;
	char *buffer_start, *buffer2_start;
	
	size_t ibl, obl;

	unsigned int frame_size;
	unsigned int b2_strlen;
	unsigned int b_size, b2_size;

	iconv_t cd;
	
	int iconv_ret;

	int x = 1;
	int y = 1;
	
	tl = 218;
	hl = 196;
	tr = 191;
	vl = 179;
	bl = 192;
	br = 217;

	frame_size = (width*height);
	b_size = (width*height)+1;
	b2_size = (b_size*4);
	
	ibl = b_size;
	obl = b2_size;

	buffer = malloc(b_size);
	buffer2 = malloc(b2_size);

	memset(buffer, 0, b_size);
	memset(buffer2, 0, b2_size);

	for(i=0; i < frame_size; i++)
	{
		if(x == 1 &&y == 1) { buffer[i] = tl; }
		else if(y==1 && x!=width) { buffer[i] = hl; }
		else if(y==1 && x==width) { buffer[i] = tr; }
		else if(x==1 && y!=height) {buffer[i] = vl; }
		else if((x>1 && x<width) && (y>1&& y<height) ) { buffer[i] = ' '; }
		else if(x==width && y!=height) { buffer[i] = vl; }
		else if(x==1 && y==height) { buffer[i] = bl; }
		else if(y==height && x != width) { buffer[i] = hl; }
		else if(y==height && x==width) { buffer[i] = br; }

		if( x == width ) { x=1; y++; }
		else{ x++; }
	}
	
	cd = iconv_open("UTF-8", "CP437");
	
	buffer_start = buffer;
	buffer2_start = buffer2;
	
	iconv_ret = iconv(cd, &buffer, &ibl, &buffer2, &obl);
	//b2_strlen = mbstowcs(NULL, buffer2, 0);
	b2_strlen = strlen(buffer2_start);

	telnet_send(telnet, "\x00\x1B\x00\x63", 4); //clear screen
	telnet_send(telnet, buffer2_start, b2_strlen);

	free(buffer_start);
	free(buffer2_start);
	iconv_close(cd);

	return 0;
}

static void _event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data)
{
	struct user_t *user = (struct user_t*)user_data;
	
	switch(ev->type)
	{
		case TELNET_EV_DATA:
			printf("TELNET_EV_DATA\n");
			break;

		case TELNET_EV_SEND:
			printf("TELNET_EV_SEND\n");
			_send(user->sock, ev->data.buffer, ev->data.size);
			break;

		case TELNET_EV_SUBNEGOTIATION:
			printf("TELNET_EV_SUBNEGOTIANTION\n");
			if(ev->sub.telopt == TELNET_TELOPT_NAWS)
			{
				user->width = (ev->sub.buffer[0] * 256) + (unsigned char)ev->sub.buffer[1];
				user->height = (ev->sub.buffer[2] * 256) + (unsigned char)ev->sub.buffer[3];

				//telnet_printf(telnet, "client width is x: %d and y: %d\n", user->width, user->height);
				printf("client width is x: %d and y: %d\n", user->width, user->height);
				_draw(user->width, user->height, telnet);
			}
			break;

		case TELNET_EV_WILL:
			printf("client will\n");
			break;

		case TELNET_EV_WONT:
			printf("client wont\n");
			break;

		case TELNET_EV_ERROR:
			close(user->sock);
			user->sock = -1;
			telnet_free(user->telnet);
			printf("TELNET_EV_ERROR\n");
			break;

		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	int listen_sock;
	struct sockaddr_in addr;
	//telnet_t *telnet;
	//struct sockaddr_in client_addr;
	socklen_t addrlen;
	int rs;
	struct pollfd pfd[2];

	char buffer[512];

	memset(&pfd, 0, sizeof(pfd));
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

	pfd[1].fd = listen_sock;
	pfd[1].events = POLLIN;

	while(1)
	{
		if(user.sock != -1)
		{
			pfd[0].fd = user.sock;
			pfd[0].events = POLLIN;
		}
		else
		{
			pfd[0].fd = -1;
			pfd[0].events = 0;
		}

		rs = poll(pfd, 2, -1);
		if(rs == -1 && errno == EINTR)
		{
			printf("Poll failed\n");
			return 1;
		}

		if(pfd[1].revents & POLLIN)
		{
		
			addrlen = sizeof(addr);

			if((rs = accept(listen_sock, (struct sockaddr*)&addr, &addrlen)) == -1)
			{
				fprintf(stderr, "accept() failed %d.\n", errno);
				return 1;
			}

			printf("Connected client: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

			user.sock = rs;
			user.telnet = telnet_init(telopts, _event_handler, 0, &user);

			telnet_negotiate(user.telnet, TELNET_DO, TELNET_TELOPT_NAWS);
		}

		if(pfd[0].revents & POLLIN)
		{
			if( (rs = recv(user.sock, buffer, sizeof(buffer), 0)) > 0)
			{
				telnet_recv(user.telnet, buffer, sizeof(buffer));
			}
			else if(rs == 0)
			{
				printf("connection closed\n");
				close(user.sock);
				telnet_free(user.telnet);
				user.sock = -1;
				break;
			}
			else if(errno != EINTR)
			{
				fprintf(stderr, "recv() failed\n");
				exit(1);
			}
		}
		//send(clientfd, "Hello, world!", sizeof("Hello, world!"), 0);

	}

	close(listen_sock);

	return 0;
}
