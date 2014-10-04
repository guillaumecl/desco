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

static void pause_desco(int signal)
{
	(void)signal;
	if (global_fb)
		pause_framebuffer(global_fb);
}

static void resume_desco(int signal)
{
	(void)signal;
	if (global_fb)
		resume_framebuffer(global_fb);
}

void setup_signals(struct framebuffer *fb)
{
	global_fb = fb;

	signal(SIGINT, interrupt_desco);
	signal(SIGTERM, interrupt_desco);
	signal(SIGSEGV, interrupt_desco);

	signal(SIGTSTP, pause_desco);
	signal(SIGCONT, resume_desco);
}
