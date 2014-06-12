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
	struct fb_fix_screeninfo fb_finfo;

	const char *fb_name = getenv("FRAMEBUFFER");
	if (!fb_name)
		fb_name = "/dev/fb0";

	fb = open(fb_name, O_RDWR);
	if (fb < 0)
	{
		perror("Error: cannot open framebuffer device");
		return NULL;
	}

	// Get variable screen information
	if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_info)) {
		printf("Error reading variable screen info.\n");
		close(fb);
		return NULL;
	}
	if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_finfo)) {
		perror("Error reading fixed screen info.\n");
		close(fb);
		return NULL;
	}
	if (fb_info.bits_per_pixel != 16 &&
		fb_info.bits_per_pixel != 32) {
		fprintf(stderr, "%d bits per pixel is not supported."
			"Only 16 or 32 are handled.\n",
			fb_info.bits_per_pixel);
		close(fb);
		return NULL;
	}
	printf("Display info %dx%d, %d bpp\n",
		fb_info.xres, fb_info.yres,
		fb_info.bits_per_pixel);
	printf("Display virt %dx%d\n",
		fb_info.xres_virtual, fb_info.yres_virtual);
	struct framebuffer *ret = malloc(sizeof(struct framebuffer));

	ret->width = fb_info.xres;
	ret->height = fb_info.yres;
	ret->bpp = fb_info.bits_per_pixel;

	ret->line_length = fb_finfo.line_length;
	ret->data_length = fb_finfo.smem_len;

	ret->u8_data = mmap(NULL, fb_finfo.smem_len, PROT_READ | PROT_WRITE,
		MAP_SHARED, fb, 0);

	if (!ret->u8_data) {
		perror("Cannot map framebuffer");
		close(fb);
		free(ret);
		return NULL;
	}
	ret->fd = fb;

	return ret;
}

void close_framebuffer(struct framebuffer *fb)
{
	close(fb->fd);
	free(fb);
}

void clear_framebuffer(struct framebuffer *fb, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int i;

	if (fb->bpp == 16)
	{
		for (i = 0; i < fb->data_length / 2; i++)
			fb->u16_data[i] = C_RGB_TO_16(r,g,b);
	}
	else
	{
		for (i = 0; i < fb->data_length / 4; i++)
			fb->u32_data[i] = C_RGB_TO_24(r,g,b);
	}
}
