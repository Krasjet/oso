/* oso.h: entrypoint of oso */
#ifndef OSO_H
#define OSO_H

#include <jack/jack.h>
#include <jack/ringbuffer.h>

typedef jack_default_audio_sample_t sample_t;

enum {
  DEF_HEIGHT = 159,
  DEF_WIDTH  = 256,
  DEF_SCALE  = 2,
  FPS        = 60,
  RB_SIZE    = 1<<16 /* must be powers of 2 */
};

typedef struct oso_t {
  /* framebuffer for drawing */
  struct btbuf *fb;
  /* ringbuffer for sending audio data */
  jack_ringbuffer_t *rb;

  /* audio parameters */
  float sr;

  /* intermediate processing for rendering */
  sample_t *minbuf;
  sample_t *maxbuf;
  size_t cursor;
  size_t sp_left;

  /* current state */
  unsigned spp;       /* samples per pixel */
  float amp_max;      /* amplitude scaling */
  int line_mode;      /* connect samples with lines? */
  int paused;         /* puased? */
} oso_t;

#endif
