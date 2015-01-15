#include "draw.h"

void _process_key(telnet_t *telnet, void *user_data, const char *buffer, unsigned int size)
{
	struct user_t *user = (struct user_t*)user_data;
	
	if(user->textmode == 0)
	{
		switch(buffer[0])
		{
			case 'c':
				printf("Open Command\n");
				//user->textmode = (user->textmode==1)?0:1;
				user->textmode = 1;
				break;
		}
	}
	else if(size==3)
	{
		switch(buffer[0])
		{
			// lol correct this please
			case 4: //ctrl+d
				printf("User CTRL+D\n");
				telnet_printf(telnet, "%sBye.", DRAW_RESET);
				close(user->sock);
				break;
				
			case 24: //ctrl+x
				printf("Close Command\n");
				user->textmode = 0;
				break;
			
		}
		if((strlen(user->command_buffer)) < USER_BUFFER) { strncpy(user->command_buffer, buffer, 1); }
	}
}
