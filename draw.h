#define DRAW_TL 218
#define DRAW_HL 196
#define DRAW_TR 191
#define DRAW_VL 179
#define DRAW_BL 192
#define DRAW_BR 217

#define DRAW_RESET "\x1B\x63"

#define DRAW_BUFFER_OFFSET(w,m,x,y,p) (((w*m)+(w*y))+p+x)-1 // -1 because buffer index
