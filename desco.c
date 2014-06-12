#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "framebuffer.h"
#include "fb_png.h"

int quit;

static void interrupt_desco(int signal)
{
	(void)signal;

	quit = 1;
}

// static void shutdown()
// {
// 	execl("/sbin/shutdown", "shutdown", "-h", "now", NULL);
// }

// static void reboot()
// {
// 	execl("/sbin/shutdown", "shutdown", "-r", "now", NULL);
// }



static void main_loop()
{
	if (isatty(STDIN_FILENO))
	{
		while(getchar() != EOF && !quit)
		{

		}
	}
	else
	{
		while(!quit)
		{
			sleep(10);
		}
	}
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	quit = 0;

	signal(SIGINT, interrupt_desco);
	signal(SIGTERM, interrupt_desco);

	struct framebuffer *fb = open_framebuffer();

	if (!fb)
		return 1;

	clear_framebuffer(fb, 0, 127, 0);

	fprintf(stderr, "Opening desco\n");

	struct png_file *desco = open_png("/root/desco/desco.png", fb);
	if (desco)
	{
		alpha_blit_png(desco, fb, 0, 0);
		close_png(desco);
	}
	else
	{
		fprintf(stderr, "Can't open desco\n");
	}
	fprintf(stderr, "Opening gentoo\n");

	struct png_file *gentoo = open_png("/root/desco/gentoo.png", fb);
	if (gentoo)
	{
		alpha_blit_png(gentoo, fb, 30, 60);
		close_png(gentoo);
	}
	else
	{
		fprintf(stderr, "Can't open gentoo\n");
	}

	main_loop();

	close_framebuffer(fb);

	return 0;

}
