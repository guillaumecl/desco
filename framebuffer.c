#include "framebuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>


struct framebuffer *open_framebuffer()
{
	int fb = 0;
	struct fb_var_screeninfo fb_info;

	const char *fb_name = getenv("FRAMEBUFFER");
	if (!fb_name)
		fb_name = "/dev/fb0";

	fb = open(fb_name, O_RDWR);
	if (fb == -1)
	{
		perror("Error: cannot open framebuffer device");
		return NULL;
	}

	// Get variable screen information
	if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_info)) {
		printf("Error reading variable screen info.\n");
		return NULL;
	}
	printf("Display info %dx%d, %d bpp\n",
		fb_info.xres, fb_info.yres,
		fb_info.bits_per_pixel);
	struct framebuffer *ret = malloc(sizeof(struct framebuffer));

	ret->width = fb_info.xres;
	ret->height = fb_info.yres;
	ret->bpp = fb_info.bits_per_pixel;

	return ret;
}

void close_framebuffer(struct framebuffer *fb)
{
	free(fb);
}
