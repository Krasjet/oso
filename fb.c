/* fb.c: 1-bit monochrome framebuffer */
#include <stdlib.h>
#include "fb.h"

static void
draw(frmbuf *fb, int x, int y)
{
  if (x >= 0 && y >= 0 && x < fb->width && y < fb->height)
    fb->buf[(fb->height-1-y)*fb->width + x] = BLACK;
}

frmbuf *
fb_new(int width, int height)
{
  frmbuf *fb = calloc(1, sizeof(frmbuf));
  if (!fb)
    return NULL;

  fb->buf = calloc(width*height, sizeof(Pixel));
  if (!fb->buf) {
    free(fb);
    return NULL;
  }
  fb_clear(fb);

  fb->width = width;
  fb->height = height;
  fb->size = width * height * sizeof(Pixel);

  return fb;
}

void
fb_free(frmbuf *fb)
{
  if (!fb)
    return;
  free(fb->buf);
  free(fb);
}

void
fb_clear(frmbuf *fb)
{
  int i;
  for (i = 0; i < fb->width*fb->height; ++i)
    fb->buf[i] = WHITE;
}

void
fb_vline(frmbuf *fb, int x, int y0, int y1)
{
  int sy = y0 < y1 ? 1 : -1;

  while (1) {
    draw(fb, x, y0);
    if (y0 == y1)
      break;
    y0 += sy;
  }
}

#define sgn(x) \
  (((x) > 0) - ((x) < 0))

void
fb_rline(frmbuf *fb, int x0, int y0, int y1)
{
  int mid, sy, cnt;
  int x1 = x0 + 1;

  /* a special case of bresenham */
  mid = abs(y1 - y0)/2;
  sy = sgn(y1 - y0);
  cnt = 0;

  while (1) {
    draw(fb, x0, y0);
    if (y0 == y1 && x0 == x1)
      break;
    if (cnt == mid)
      x0++;
    y0 += sy;
    cnt++;
  }
}
