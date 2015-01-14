struct user_t
{
	int sock;
	telnet_t *telnet;

	int width, height;
	int textmode;
	char command_buffer[USER_BUFFER];
	//int buffermode =0; // commandbuffer, postbuffer, replybuffer etc..
};
