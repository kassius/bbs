void _send(int sock, const char *buffer, unsigned int size)
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
