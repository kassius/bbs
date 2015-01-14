/*extern static int _draw_buffer(int width, int height, telnet_t *telnet, user_t *user_data)
{
	int frame_size;
	struct user_t *user = (struct user_t*)user_data; 
	frame_size = (width-2)*((height-2)/2);
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
}*/

extern int _draw(int width, int height, telnet_t *telnet, void *user_data)
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
	
	struct user_t *user = (struct user_t*)user_data; 
	
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
	
	if(user->textmode)
	{
	}
	
	cd = iconv_open("UTF-8", "CP437");
	
	buffer_start = buffer;
	buffer2_start = buffer2;
	
	iconv_ret = iconv(cd, &buffer, &ibl, &buffer2, &obl);
	//b2_strlen = mbstowcs(NULL, buffer2, 0);
	b2_strlen = strlen(buffer2_start);

	telnet_send(telnet, "\x00\x1B\x00\x63", 4); //clear screen
	telnet_send(telnet, buffer2_start, b2_strlen);
	telnet_send(telnet, "\x00\x1B\x00\x5b\x00\x32\x00\x3b\x00\x32\x00\x48", 12); //cursor position
	telnet_send(telnet, "Hello dude", 10);

	free(buffer_start);
	free(buffer2_start);
	iconv_close(cd);

	return 0;
}
