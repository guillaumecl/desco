#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <tslib.h>

#include "framebuffer.h"
#include "fb_png.h"

struct framebuffer *fb;

static void interrupt_desco(int signal)
{
	(void)signal;
	close_framebuffer(fb);
	exit(0);
}

static void shutdown()
{
	execl("/sbin/shutdown", "shutdown", "-h", "now", NULL);
}

// static void reboot()
// {
// 	execl("/sbin/shutdown", "shutdown", "-r", "now", NULL);
// }



static void main_loop()
{
	struct ts_sample samp;
	struct tsdev *ts;
	int retry = 5;

	do
	{
		ts = ts_open ("/dev/input/touchscreen", 0);
		if (ts == NULL && retry > 1 )
		{
			sleep(1);
		}
	} while (ts == NULL && --retry > 0);

	if (!ts) {
		perror ("ts_open");
		return;
	}

	if (ts_config(ts)) {
		perror("ts_config");
		return;
	}

	while (1) {
		int ret;

		ret = ts_read(ts, &samp, 1);
		fprintf(stderr, "Ret: %d\n", ret);
		if (ret == 1)
		{
			shutdown();
		}

	}
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	signal(SIGINT, interrupt_desco);
	signal(SIGTERM, interrupt_desco);

	fb = open_framebuffer();

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
