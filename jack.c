/* jack.c: interop with jack */
#include "jack.h"
#include "util.h"
#include "oso.h"
#include <stdlib.h>

static int
on_process(jack_nframes_t nframes, void *arg)
{
  const sample_t *in;
  oso_t *o = (oso_t *)arg;

  in = jack_port_get_buffer(o->port_in, nframes);
  /* copy samples to ringbuffer.
   * write might fail when buffer is full, but we don't care */
  jack_ringbuffer_write(o->rb, (const char *)in, nframes*sizeof(sample_t));
  return 0;
}

/* die when jack server shuts down */
static void
on_shutdown(void *arg)
{
  (void)arg;
  die("jack server is down, exiting...");
}

void
jack_init(oso_t *o)
{
  o->client = jack_client_open("oso", JackNoStartServer, NULL);
  if (!o->client)
    die("fail to open jack client");

  /* set up callbacks */
  jack_on_shutdown(o->client, on_shutdown, NULL);
  if (jack_set_process_callback(o->client, on_process, o))
    die("fail to set up jack process callback");

  /* register input port */
  o->port_in = jack_port_register(o->client, "in",
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);
  if (!o->port_in)
    die("fail to register jack input port");

  /* we will use a ringbuffer to pass samples from audio thread to gui */
  o->rb = jack_ringbuffer_create(sizeof(sample_t) * RB_SIZE);
  if (!o->rb)
    die("fail create ringbuffer");
  if (jack_ringbuffer_mlock(o->rb))
    die("fail to lock memory");

  if (jack_activate(o->client))
    die("fail to activate jack client");
}

void
jack_finish(oso_t *o)
{
  if (!o)
    return;

  jack_deactivate(o->client);
  jack_client_close(o->client);

  jack_ringbuffer_free(o->rb);
}
