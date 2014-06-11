#include "framebuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>

#include <errno.h>

static void disable_blink()
{
	int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY|O_TRUNC);
	if (fd < 0)
		return;

	while (write(fd, "0", 1) == EINTR)
	{
	}

	close(fd);
}

struct framebuffer *open_framebuffer()
{
	int fb = 0;
	struct fb_var_screeninfo fb_info;
	struct fb_fix_screeninfo fb_finfo;

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
	if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_finfo)) {
		printf("Error reading fixed screen info.\n");
		return NULL;
	}
	printf("Display info %dx%d, %d bpp\n",
		fb_info.xres, fb_info.yres,
		fb_info.bits_per_pixel);
	struct framebuffer *ret = malloc(sizeof(struct framebuffer));

	ret->width = fb_info.xres;
	ret->height = fb_info.yres;
	ret->bpp = fb_info.bits_per_pixel;

	ret->u8_data = mmap(NULL, fb_finfo.smem_len, PROT_READ | PROT_WRITE,
		MAP_SHARED, fb, 0);

	if (!ret->u8_data) {
		perror("Cannot map framebuffer");
		close(fb);
		free(ret);
		return NULL;
	}
	disable_blink();

	close(fb);
	unsigned int i;

	for (i = 0; i < fb_finfo.smem_len/2; i ++)
		ret->u16_data[i] = C_RGB_TO_16(30,0,0);

	return ret;
}

void close_framebuffer(struct framebuffer *fb)
{
	free(fb);
}
