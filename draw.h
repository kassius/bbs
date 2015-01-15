#define DRAW_TL 218
#define DRAW_HL 196
#define DRAW_TR 191
#define DRAW_VL 179
#define DRAW_BL 192
#define DRAW_BR 217

#define DRAW_TLa 71
#define DRAW_HLa 82
#define DRAW_TRa 73
#define DRAW_VLa 74
#define DRAW_BLa 75
#define DRAW_BRa 76

#define DRAW_RESET "\x1B\x63"
#define DRAW_MOVE_CURSOR(xx,yy) "\x1B[xx;yyH" //<ESC>[{x};{y}H;

unsigned char *draw_move_cursor(unsigned char *string, int x, int y)
{
	unsigned char cX[5], cY[5];
	
	sprintf(cX, "%d", x);
	sprintf(cY, "%d", y);
	
	sprintf(string, "%x[%s,%sH", 0x1B, cX, cY );
}

/*unsigned char *draw_move_cursor_iconv(unsigned char *string, int x, int y)
{
	unsigned char cX[5], cY[5];
	
	sprintf(cX, "%d", x);
	sprintf(cY, "%d", y);
	
	sprintf(string, "%x%x%x%x%x%x%x%x%x%x%x%x",
		0x00,
		0x1B, // ESC
		0x00,
		0x5B, // '['
		0x00,
		0x32, //makeme
		0x00,
		0x3b, // ','
		0x00,
		0x32,
		0x00,
		0x48 // 'H'
		)
}*/
