#include <libtelnet.h>
#include <math.h>

#include "draw.h"

void _draw_textbox(char *screen_buffer, int screen_width, int screen_height)
{
	int i=0;
	int x=1;
	int y=1;
	
	int padding=1;
	
	int textbox_width = (screen_width-2);
	int textbox_height = ((screen_height-2)/2);
	int frame_size = (textbox_width * textbox_height);
	
	double sh = screen_height;
	
	int screen_vmiddle = floor(sh/2); //vertical middle
	
	for(i=0; i < frame_size; i++)
	{
		if(x == 1 && y == 1) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_TL; }
		else if(y==1 && x!=textbox_width) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_HL; }
		else if(y==1 && x==textbox_width) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_TR; }
		else if(x==1 && y!=textbox_height) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_VL; }
		else if((x>1 && x<textbox_width) && (y>1 && y<textbox_height) ) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = ' '; }
		else if(x==textbox_width && y!=textbox_height) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_VL; }
		else if(x==1 && y==textbox_height) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_BL; }
		else if(y==textbox_height && x!=textbox_width) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_HL; }
		else if(y==textbox_height && x==textbox_width) { screen_buffer[DRAW_BUFFER_OFFSET(screen_width,screen_vmiddle,x,y,padding)] = DRAW_BR; }

		if( x == textbox_width ) { x=1; y++; }
		else{ x++; }
	}
	
	return;
}

static int _draw(int width, int height, telnet_t *telnet, void *user_data)
{
	int i;

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
		if(x == 1 &&y == 1) { buffer[i] = DRAW_TL; }
		else if(y==1 && x!=width) { buffer[i] = DRAW_HL; }
		else if(y==1 && x==width) { buffer[i] = DRAW_TR; }
		else if(x==1 && y!=height) {buffer[i] = DRAW_VL; }
		else if((x>1 && x<width) && (y>1&& y<height)) { buffer[i] = ' '; }
		else if(x==width && y!=height) { buffer[i] = DRAW_VL; }
		else if(x==1 && y==height) { buffer[i] = DRAW_BL; }
		else if(y==height && x!=width) { buffer[i] = DRAW_HL; }
		else if(y==height && x==width) { buffer[i] = DRAW_BR; }

		if( x == width ) { x=1; y++; }
		else{ x++; }
	}

	if(user->textmode)
	{
		_draw_textbox(buffer, width, height);
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
