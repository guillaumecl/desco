#include <signal.h>
#include <stdlib.h>

#include "graphics/framebuffer.h"


static struct framebuffer *global_fb;

static void interrupt_desco(int signal)
{
	(void)signal;
	if (global_fb)
		close_framebuffer(global_fb);
	exit(0);
}

void setup_signals(struct framebuffer *fb)
{
	global_fb = fb;

	signal(SIGINT, interrupt_desco);
	signal(SIGTERM, interrupt_desco);
}
