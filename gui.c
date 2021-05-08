/* gui.c: gui component of oso, can be replaced with other backends */
#include <stdio.h>
#include <SDL2/SDL.h>

#include "oso.h"
#include "util.h"
#include "fb.h"

static SDL_Window *win;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static Uint32 framedelay;

static void
handle_keypress(oso_t *o, int key)
{
  switch (key) {
  case SDLK_SPACE:         /* toggle pause */
    o->paused ^= 1;
    break;
  case SDLK_0:             /* toggle line mode */
    o->line_mode ^= 1;
    break;
  case SDLK_MINUS:         /* time zoom in */
    o->spp /= 2;
    if (o->spp < 1)
      o->spp = 1;
    break;
  case SDLK_EQUALS:        /* time zoom out */
    o->spp *= 2;
    break;
  /* XXX: does it make sense to use db scale here? */
  case SDLK_LEFTBRACKET:   /* amp zoom in */
    o->amp_max -= 0.1;
    if (o->amp_max < 0.1)
      o->amp_max = 0.1;
    break;
  case SDLK_RIGHTBRACKET:  /* amp zoom out */
    o->amp_max += 0.1;
    if (o->amp_max > 1.5)
      o->amp_max = 1.5;
    break;
  }
}

void
gui_init(int width, int height, int scale, int fps)
{
  int rc;

  rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0)
    die("fail to init sdl");

  rc = SDL_CreateWindowAndRenderer(width*scale, height*scale,
                                   SDL_WINDOW_ALLOW_HIGHDPI,
                                   &win, &renderer);
  if (rc != 0)
    die("fail to create window");
  SDL_SetWindowTitle(win, "oso");

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32,
                              SDL_TEXTUREACCESS_STREAMING,
                              width, height);
  if (!texture)
    die("fail to create texture");

  framedelay = 1000/fps;
}

int
gui_handle_events(oso_t *o) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return 1;
    case SDL_KEYDOWN:
      handle_keypress(o, event.key.keysym.sym);
      break;
    }
  }
  return 0;
}

void
gui_finish(void)
{
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void
gui_redraw(struct frmbuf *fb) {
  Pixel *texbuf;
  int pitch;
  static Uint32 ntick = 0;

  /* cap fps to prevent burning cpu */
  Uint32 tick = SDL_GetTicks();
  if (tick < ntick)
    SDL_Delay(ntick - tick);
  ntick = SDL_GetTicks() + framedelay;

  /* copy fb to texture */
  SDL_LockTexture(texture, NULL, (void **)&texbuf, &pitch);
  memcpy(texbuf, fb->buf, fb->size);
  SDL_UnlockTexture(texture);

  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
