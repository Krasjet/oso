/* gui.h: gui component of oso, can be replaced with other backends */
#ifndef OSO_GUI_H
#define OSO_GUI_H

struct oso_t;
struct btbuf;

void gui_init(int width, int height, int scale, int fps);
void gui_finish(void);
/* returns 1 on quit event, 0 otherwise */
int gui_handle_events(oso_t *o);
void gui_redraw(struct btbuf *fb);

#endif
