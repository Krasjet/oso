/* jack.h: interop with jack */
#ifndef OSO_JACK_H
#define OSO_JACK_H 

struct oso_t;

void jack_init(struct oso_t *o);
void jack_finish(struct oso_t *o);

#endif
