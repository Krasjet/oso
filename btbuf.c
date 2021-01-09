/* btbuf.c: a tiny monochrome framebuffer library */
#include <stdlib.h>

#include "btbuf.h"

#define draw(x, y) \
  buf->buf[(y)*buf->width + (x)] = BT_BLACK

/* clamp val to [a,b] */
#define clamp(val, a, b) \
  val = val < (a) ? (a) : (val > (b) ? (b) : val)

#define sgn(x) \
  (((x) > 0) - ((x) < 0))

btbuf *
btbuf_new(int width, int height)
{
  btbuf *buf = calloc(1, sizeof(btbuf));
  if (!buf)
    return NULL;

  buf->buf = calloc(width*height, sizeof(bt_pixel));
  if (!buf->buf) {
    free(buf);
    return NULL;
  }
  btbuf_clear(buf);

  buf->width = width;
  buf->height = height;
  buf->size = width * height * sizeof(bt_pixel);

  return buf;
}

void
btbuf_free(btbuf *buf)
{
  if (!buf)
    return;
  free(buf->buf);
  free(buf);
}

void
btbuf_clear(btbuf *buf)
{
  int i;
  for (i = 0; i < buf->width*buf->height; ++i)
    buf->buf[i] = BT_WHITE;
}

void 
btbuf_vline(btbuf *buf, int x, int y0, int y1)
{
  int y;

  clamp(y0, 0, buf->height-1);
  clamp(y1, 0, buf->height-1);

  if (y0 > y1) {
    int tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  for (y = y0; y <= y1; ++y)
    draw(x, y);
}

void 
btbuf_rline(btbuf *buf, int x0, int y0, int y1)
{
  int mid, sy, cnt;
  int x1 = x0 + 1;

  clamp(y0, 0, buf->height-1);
  clamp(y1, 0, buf->height-1);

  /* a special case of bresenham */
  mid = abs(y1 - y0)/2;
  sy = sgn(y1 - y0);
  cnt = 0;

  while (1) {
    draw(x0, y0);
    if (y0 == y1 && x0 == x1)
      break;
    if (cnt == mid)
      x0++;
    y0 += sy;
    cnt++;
  }
}
