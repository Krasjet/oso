/* oso.h: entrypoint of oso */
#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>

#include "oso.h"
#include "jack.h"
#include "gui.h"
#include "fb.h"
#include "util.h"

static volatile int should_exit = 0;

static void
on_signal(int signum)
{
  (void)signum;
  should_exit = 1;
}

static int
done(oso_t *o)
{
  return gui_handle_events(o) || should_exit;
}

static void
reset_cursor(oso_t *o)
{
  o->cursor = 0;
  o->sp_left = o->spp;
  o->maxbuf[0] = -o->amp_max;
  o->minbuf[0] =  o->amp_max;
}

static void
oso_init(oso_t *o, int width, int height, int scale)
{
  o->fb = fb_new(width, height);
  if (!o->fb)
    die("fail to allocate framebuffer");

  o->maxbuf = calloc(width, sizeof(sample_t));
  o->minbuf = calloc(width, sizeof(sample_t));
  if (!o->maxbuf || !o->minbuf)
    die("fail to init max/min buffer");

  gui_init(width, height, scale, FPS);
  jack_init(o);

  /* default parameters */
  o->spp = 8;
  o->amp_max = 1;
  o->line_mode = 1;
  o->paused = 0;
  reset_cursor(o);
}

static void
oso_finish(oso_t *o)
{
  jack_finish(o);
  gui_finish();
}

static void
process_samples(oso_t *o)
{
  static sample_t buf[RB_SIZE];

  size_t avail, size, i;
  int width = o->fb->width;
  size_t max_size = width * o->spp * sizeof(sample_t);

  avail = jack_ringbuffer_read_space(o->rb);
  if (avail <= 0)
    return;

  if (avail >= max_size) {
    /* if we get too many samples, only take from the end */
    jack_ringbuffer_read_advance(o->rb, avail - max_size);
    avail = max_size;
    /* reset cursor to prevent a split in the middle
     * of screen */
    reset_cursor(o);
  } else if (o->spp*width/o->sr < 4.f/FPS) {
    /* disable scrolling for low spp (mainly for 4<spp<8).
     * this might cause some delay, but for low spp,
     * it's barely noticeable. */

    /* in this case, only update screen when buffer is full
     * to prevent splitting */
    return;
  }

  size = jack_ringbuffer_read(o->rb, (char*)buf, avail);
  if (size <= 0)
    return;
  /* convert # bytes to # samples */
  size /= sizeof(sample_t);

  for (i = 0; i < size; ++i) {
    if (o->sp_left == 0) {
      /* proceed to next pixel */
      o->cursor = (o->cursor + 1) % width;
      o->maxbuf[o->cursor] = -o->amp_max;
      o->minbuf[o->cursor] =  o->amp_max;
      o->sp_left = o->spp;
    }

    /* update minbuf and maxbuf for later drawing */
    if (buf[i] < o->minbuf[o->cursor])
      o->minbuf[o->cursor] = buf[i];
    if (buf[i] > o->maxbuf[o->cursor])
      o->maxbuf[o->cursor] = buf[i];

    --o->sp_left;
  }
}

/* map interval [x0, x1] -> [y0, y1]
 * note: it does no overflow checking. */
#define map(val, x0, x1, y0, y1) \
  ((y0) + ((y1)-(y0)) * ((val)-(x0))/((float)(x1)-(x0)))

static void
render_fb(oso_t *o)
{
  size_t x;
  float amp = o->amp_max;
  sample_t prevmax = 0, prevmin = 0;
  size_t w = o->fb->width, h = o->fb->height;

  fb_clear(o->fb);

  for (x = 0; x < w; ++x)  {
    sample_t max = o->maxbuf[x];
    sample_t min = o->minbuf[x];
    int ymax = map(max, -amp, amp, 0, h-1);
    int ymin = map(min, -amp, amp, 0, h-1);

    if (o->line_mode && x > 0 && o->cursor != x-1) {
      /* connect any jumps, except at cursor */
      int yprevmax = map(prevmax, -amp, amp, 0, h-1);
      int yprevmin = map(prevmin, -amp, amp, 0, h-1);
      if (prevmax < min)
        fb_rline(o->fb, x-1, yprevmax, ymin);
      else if (prevmin > max)
        fb_rline(o->fb, x-1, yprevmin, ymax);
    }

    /* connect min and max at current sample */
    fb_vline(o->fb, x, ymin, ymax);
    prevmax = max;
    prevmin = min;
  }
}

static void
usage(const char *name)
{
  fprintf(stdout, "usage: %s [-W width] [-H height] [-S scale] [-h]\n", name);
}

int
main(int argc, char *argv[])
{
  oso_t o;
  int width = DEF_WIDTH, height = DEF_HEIGHT;
  int scale = DEF_SCALE;

  int c;
  while ((c = getopt(argc, argv, "W:H:S:h")) != -1) {
    switch (c) {
    case 'W':
      width = atoi(optarg);
      if (width <= 0)
        die("invalid width");
      break;
    case 'H':
      height = atoi(optarg);
      if (height <= 0)
        die("invalid height");
      break;
    case 'S':
      scale = atoi(optarg);
      if (scale <= 0)
        die("invalid scale");
      break;
    case 'h':
      usage(argv[0]);
      return 0;
    default:
      usage(argv[0]);
      return 1;
    }
  }

  oso_init(&o, width, height, scale);

  /* catch signal for grace shutdown */
  signal(SIGINT, on_signal);
  signal(SIGTERM, on_signal);
#ifndef _WIN32
  signal(SIGQUIT, on_signal);
  signal(SIGHUP, on_signal);
#endif

  while (!done(&o)) {
    process_samples(&o);
    if (!o.paused)
      render_fb(&o);
    gui_redraw(o.fb);
  }

  oso_finish(&o);
  return 0;
}
