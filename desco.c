#include <unistd.h>
#include <stdio.h>

#include <tslib.h>

#include <poll.h>

#include "graphics/framebuffer.h"
#include "graphics/fb_png.h"
#include "signals.h"

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
	uint32_t color = C_RGB_TO_24(255,255,255);
	if(f && fscanf(f, "%lf", &d))
	{
		fb_printf(fb, x, y, color, 0, "Temp: %lf°", d/1000);
		fclose(f);
	}
	else
		fb_print(fb, x, y, color, 0, "Temp: unknown");
}

static void main_loop(struct framebuffer *fb)
{
	struct ts_sample samp;
	struct tsdev *ts;
	int retry = 5;
	struct pollfd polls;

	do {
		ts = ts_open ("/dev/input/touchscreen", 0);
		if (ts == NULL && retry > 1 ) {
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
				fb_print(fb, 0, 0, C_RGB_TO_24(255,0,0), 0, "Initiating shutdown...");
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
	clear_framebuffer(fb, 0,50, 0);

	fprintf(stderr, "Opening desco\n");

	struct png_file *desco = open_png("/root/desco/desco.png", fb);
	if (desco) {
		alpha_blit_png(desco, fb, 0, 0);
		close_png(desco);
	} else {
		fprintf(stderr, "Can't open desco\n");
	}
	fprintf(stderr, "Opening gentoo\n");

	struct png_file *gentoo = open_png("/root/desco/gentoo.png", fb);
	if (gentoo) {
		alpha_blit_png(gentoo, fb, 30, 60);
		close_png(gentoo);
	} else {
		fprintf(stderr, "Can't open gentoo\n");
	}

	fb_print(fb, 10, 20, C_RGB_TO_24(255,0,0), 0, "Hey, dood!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	fb_print(fb, 10, 40, C_RGB_TO_24(0,255,0), 0, "ひらがな");

	fb_print(fb, 10, 48, C_RGB_TO_24(0,0,255), 0, "ひらがな");

	main_loop(fb);

	close_framebuffer(fb);

	return 0;

}
