/* util.c: common utilities */
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <SDL2/SDL.h>
#endif

void
die(const char *msg)
{
#ifdef _WIN32
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, NULL);
#else
  fprintf(stderr, "[error] %s\n", msg);
#endif
  exit(1);
}
