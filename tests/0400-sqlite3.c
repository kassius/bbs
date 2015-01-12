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

#include <sqlite3.h>

//#include <libtelnet.h>

#define	DATABASE_FILE	"sqlite3.db"

int main()
{
	sqlite3 *db;
	
	int rc;
	
	rc = sqlite3_open(DATABASE_FILE, &db);
	if(rc)
	{
		printf("ERROR OPENING DB\n\n");
		return 1;
	}
	else
	{
		printf("DB SUCESSFULLY OPEN\n\n");
	}
	
	sqlite3_close(db);
	return 0;
}
