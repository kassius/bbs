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

//#include <libtelnet.h>

#define 	SIZE	32

int main()
{
	char *buffer, *buffer2;
	char *buffer2_start;
	
	size_t ibl, obl;
	int buffer_size, buffer2_size;
	
	iconv_t cd;
	
	int iconv_ret;
	
	buffer_size = SIZE+1;
	buffer2_size = (SIZE*2)+1;
	
	ibl = SIZE+1;
	obl= (SIZE*2)+1;
	
	buffer = calloc(buffer_size, sizeof(char));
	buffer2 = calloc(buffer2_size, sizeof(char));
	
	memset(buffer, 0, buffer_size);
	memset(buffer2, 0, buffer2_size);
	
	//strcat(buffer, "HELLO");
	buffer[0] = (unsigned char)218;
	cd = iconv_open("UTF-8", "CP437");

	buffer2_start = buffer2;
	
	iconv_ret = iconv(cd, &buffer, &buffer_size, &buffer2, &buffer2_size);
	
	printf("res: %i bytes - \"%s\"\n\n", iconv_ret, buffer2_start);
	
	iconv_close(cd);
	return 0;
}
