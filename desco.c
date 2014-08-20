#include <unistd.h>
#include <stdio.h>

#include <tslib.h>

#include <poll.h>

#include "graphics/framebuffer.h"
#include "graphics/fb_png.h"
#include "signals.h"

static uint32_t backcolor;
static uint32_t textcolor;

static void shutdown()
{
	execl("/sbin/shutdown", "shutdown", "-h", "now", NULL);
}

// static void reboot()
// {
// 	execl("/sbin/shutdown", "shutdown", "-r", "now", NULL);
// }


static void print_temp(struct framebuffer *fb, unsigned int x, unsigned int y)
{
	FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	double d;
	uint32_t color = rgb_to_24(255,255,255);
	if(f && fscanf(f, "%lf", &d))
	{
		fb_printf(fb, x, y, color, backcolor, "Temp: %lf°", d/1000);
		fclose(f);
	}
	else
		fb_print(fb, x, y, color, backcolor, "Temp: unknown");
}

static void main_loop(struct framebuffer *fb)
{
	struct ts_sample samp;
	struct tsdev *ts;
	int retry = 5;
	struct pollfd polls;

	fb_print(fb, 0, 0, textcolor, backcolor, "Trying to open touchscreen...");
	do {
		ts = ts_open ("/dev/input/touchscreen", 0);
		if (ts == NULL && retry > 1 ) {
			fb_print(fb, 0, 0, textcolor, backcolor, "Trying to open touchscreen... Trying again… ");
			sleep(1);
			fb_print(fb, 0, 0, textcolor, backcolor, "Trying to open touchscreen... Trying again !");
		}
	} while (ts == NULL && --retry > 0);

	if (!ts) {
		fb_print(fb, 0, 0, textcolor, backcolor, "Cannot open touchscreen.                           ");
		perror ("ts_open");
		return;
	}

	fb_print(fb, 0, 0, textcolor, backcolor, "Configuring touchscreen...                           ");
	if (ts_config(ts)) {
		fb_print(fb, 0, 0, textcolor, backcolor, "ts_config failed.                           ");
		perror("ts_config");
		return;
	}

	fb_print(fb, 0, 0, textcolor, backcolor, "                                            ");

	polls.events = POLLIN;
	polls.fd = ts_fd(ts);

	while (1) {
		print_temp(fb, 0, 200);
		int ret;

		ret = poll(&polls, 1, 1000);
		if (polls.revents) {
			ret = ts_read(ts, &samp, 1);
			fprintf(stderr, "Ret: %d\n", ret);
			if (ret == 1) {
				fb_print(fb, 0, 0, textcolor, backcolor, "Initiating shutdown...");
				shutdown();
			}
		}
	}
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	struct framebuffer *fb = open_framebuffer();

	if (!fb)
		return 1;

	setup_signals(fb);

	backcolor = rgb_to_24(0,50,0);
	textcolor = rgb_to_24(255,0,0);

	clear_framebuffer(fb, 0, 50, 0);

	fprintf(stderr, "Opening desco\n");

	fb_print(fb, 10, 20, textcolor, backcolor, "Opening desco…");
	struct png_file *desco = open_png("/root/desco/desco.png", fb);
	if (desco) {
		alpha_blit_png(desco, fb, 0, 0);
		close_png(desco);
	} else {
		fprintf(stderr, "Can't open desco\n");
	}

	fb_print(fb, 10, 20, textcolor, backcolor, "Opening gentoo…");
	fprintf(stderr, "Opening gentoo\n");

	struct png_file *gentoo = open_png("/root/desco/gentoo.png", fb);
	if (gentoo) {
		alpha_blit_png(gentoo, fb, 30, 60);
		close_png(gentoo);
	} else {
		fprintf(stderr, "Can't open gentoo\n");
	}

	fb_print(fb, 10, 20, rgb_to_24(255,0,0), backcolor, "Hey, dood!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	fb_print(fb, 10, 40, rgb_to_24(0,255,0), backcolor, "ひらがな");

	fb_print(fb, 10, 48, rgb_to_24(0,0,255), backcolor, "ひらがな");

	main_loop(fb);

	close_framebuffer(fb);

	return 0;

}
