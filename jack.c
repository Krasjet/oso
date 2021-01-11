/* jack.c: interop with jack */
#include <stdlib.h>

#include "jack.h"
#include "util.h"
#include "oso.h"

static jack_client_t *client;
static jack_port_t *port_in;

static int
on_process(jack_nframes_t nframes, void *arg)
{
  const sample_t *in;
  jack_ringbuffer_t *rb = arg;

  in = jack_port_get_buffer(port_in, nframes);
  /* copy samples to ringbuffer.
   * write might fail when buffer is full, but we don't care */
  jack_ringbuffer_write(rb, (const char *)in, nframes*sizeof(sample_t));
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
  client = jack_client_open("oso", JackNoStartServer, NULL);
  if (!client)
    die("fail to open jack client");

  /* register input port */
  port_in = jack_port_register(client, "in",
                               JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsInput, 0);
  if (!port_in)
    die("fail to register jack input port");

  /* we will use a ringbuffer to pass samples from audio thread to gui */
  o->rb = jack_ringbuffer_create(sizeof(sample_t) * RB_SIZE);
  if (!o->rb)
    die("fail create ringbuffer");
  if (jack_ringbuffer_mlock(o->rb))
    die("fail to lock memory");

  /* set up callbacks */
  jack_on_shutdown(client, on_shutdown, NULL);
  if (jack_set_process_callback(client, on_process, o->rb))
    die("fail to set up jack process callback");

  if (jack_activate(client))
    die("fail to activate jack client");
}

void
jack_finish(oso_t *o)
{
  jack_deactivate(client);
  jack_client_close(client);

  jack_ringbuffer_free(o->rb);
}
