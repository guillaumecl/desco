#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "graphics/framebuffer.h"
#include "graphics/fb_png.h"
#include "signals.h"

#include "input/touchscreen.h"
#include "input/stdin.h"
#include "input/input_event.h"
#include "input/input.h"

static color_t backcolor;
static color_t textcolor;

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

struct loop_data
{
	struct framebuffer *fb;
	int stop;
};

static void shutdown()
{
	execl("/usr/bin/systemctl", "poweroff", NULL);
}

static void reboot()
{
	execl("/usr/bin/systemctl", "reboot", NULL);
}


static void print_temp(struct framebuffer *fb, unsigned int x, unsigned int y)
{
	FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	double d;
	color_t color = rgb_to_native(fb, 255,255,255);
	if(f && fscanf(f, "%lf", &d))
	{
		fb_printf(fb, x, y, color, backcolor, "Temp: %lf°", d/1000);
		fclose(f);
	}
	else
		fb_print(fb, x, y, color, backcolor, "Temp: unknown");
}

static void parse_input(struct input *in, void *d)
{
	struct loop_data *data = (struct loop_data*)d;
	struct framebuffer *fb = data->fb;

	if (in->type == event_key) {
		data->stop = 1;
		return;
	}

	unsigned int x = max(in->mouse.x, 0);
	unsigned int y = max(in->mouse.y, 0);

	fprintf(stderr, "x: %d, y: %d\n", x, y);
	if (x >= fb->width - 30 && y <= 30) {
		fb_print(fb, 0, 0, textcolor, backcolor, "Initiating shutdown...");
		shutdown();
	} else if (x >= fb->width - 30 && y >= fb->height - 30) {
		fb_print(fb, 0, 0, textcolor, backcolor, "Initiating reboot...");
		reboot();
	} else if (x <= 30 && y <= 30) {
		data->stop = 1;
	}
}

static void main_loop(struct framebuffer *fb)
{
	struct input_queue *queue;
	struct loop_data data;

	queue = alloc_input_queue();
	if (!queue) {
		fb_print(fb, 0, 0, textcolor, backcolor, "Cannot create input queue.                           ");
		return;
	}

	register_input(queue, open_touchscreen("/dev/input/touchscreen"));
	register_input(queue, open_stdin());

	data.fb = fb;
	data.stop = 0;
	while (!data.stop) {
		print_temp(fb, 0, 200);
		poll_input(queue, parse_input, 1000, &data);
	}

	free_input_queue(queue);
}

void setup_directory()
{
	const char *path = getenv("DESCO_PATH");
	if (!path)
		return;

	if(chdir(path))
		perror("Cannot set the current directory");
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	setup_directory();

	struct framebuffer *fb = open_framebuffer();

	if (!fb)
		return 1;

	setup_signals(fb);

	backcolor = rgb_to_native(fb, 0,50,0);
	textcolor = rgb_to_native(fb, 255,0,0);

	clear_framebuffer(fb, backcolor);

	fprintf(stderr, "Opening desco\n");

	fb_print(fb, 10, 20, textcolor, backcolor, "Opening desco…");
	struct png_file *desco = open_png("desco.png", fb);
	if (desco) {
		alpha_blit_png(desco, fb, 0, 0);
		close_png(desco);
	} else {
		fprintf(stderr, "Can't open desco\n");
	}

	fb_print(fb, 10, 20, textcolor, backcolor, "Opening gentoo…");
	fprintf(stderr, "Opening gentoo\n");

	struct png_file *gentoo = open_png("gentoo.png", fb);
	if (gentoo) {
		alpha_blit_png(gentoo, fb, 30, 60);
		close_png(gentoo);
	} else {
		fprintf(stderr, "Can't open gentoo\n");
	}

	fb_print(fb, 10, 20, rgb_to_native(fb, 255,0,0), backcolor, "Hey, dood!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

	fb_print(fb, 10, 40, rgb_to_native(fb, 0,255,0), backcolor, "ひらがな");

	fb_print(fb, 10, 48, rgb_to_native(fb, 0,0,255), backcolor, "ひらがな");

	main_loop(fb);

	close_framebuffer(fb);

	return 0;

}
