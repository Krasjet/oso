/* fb.h: 1-bit monochrome framebuffer */
#ifndef FB_H
#define FB_H

#include <stdint.h>

/* different backends might use different pixel format */
typedef uint32_t Pixel;

#define BLACK 0x00000000
#define WHITE 0xffffffff

typedef struct frmbuf {
  Pixel *buf;
  int width;
  int height;
  int size;
} frmbuf;

frmbuf * fb_new(int width, int height);
void fb_free(frmbuf *fb);
void fb_clear(frmbuf *fb);

/* draw vertical line from (x,y0) to (x,y1) */
void fb_vline(frmbuf *fb, int x, int y0, int y1);
/* draw line from (x,y0) to (x+1,y1) */
void fb_rline(frmbuf *fb, int x, int y0, int y1);

#endif
