#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H
void fb_init();
void drawPixel(int x, int y, unsigned char attr);
void drawChar(unsigned char ch, int x, int y, unsigned char attr);
void drawString(int x, int y, char *s, unsigned char attr);
void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr);
void wait_msec(unsigned int n);
void debugstr(char *str);
void debugcrlf(void);
void debugch(unsigned char b);
void debughex(unsigned int d);
#endif