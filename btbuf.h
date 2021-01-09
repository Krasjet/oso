/* btbuf.h: a tiny monochrome framebuffer library */
#ifndef BTBUF_H
#define BTBUF_H

#include <stdint.h>

/* different backends might use different pixel format */
typedef uint32_t bt_pixel;

#define BT_BLACK 0x00000000
#define BT_WHITE 0xffffffff

typedef struct btbuf {
  bt_pixel *buf;
  int width;
  int height;
  int size;
} btbuf;

btbuf * btbuf_new(int width, int height);
void btbuf_free(btbuf *buf);
void btbuf_clear(btbuf *buf);
/* draw vertical line from (x,y0) to (x,y1)
 * ASSUMES: 0<=x<width */
void btbuf_vline(btbuf *buf, int x, int y0, int y1);
/* draw line from (x,y0) to (x+1,y1) 
 * ASSUMES: 0<=x<width-1 */
void btbuf_rline(btbuf *buf, int x, int y0, int y1);

#endif
