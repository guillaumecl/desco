#include "framebuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include <sys/mman.h>
#include <stropts.h>


static int init_term()
{
	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int console_fd;

	console_fd = open("/dev/tty", O_RDWR | O_NDELAY);
	if (console_fd >= 0)
	{

		if (ioctl(console_fd, KDSETMODE, KD_GRAPHICS) < 0) {
			perror("KDSETMODE");
			close(console_fd);
			return -1;
		}
	}

	if (isatty(STDOUT_FILENO))
	{
		if (!getenv("VT"))
			return console_fd;
		log_file = open("/tmp/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);
	}
	else
	{
		log_file = open("/var/log/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);
	}
	if (log_file < 0) {
		perror("Cannot open log file for writing");

		return console_fd;
	}

	if (dup2(log_file, STDERR_FILENO) != STDERR_FILENO ||
		dup2(log_file, STDOUT_FILENO) != STDOUT_FILENO) {
		perror("Unable to redirect output");
		return console_fd;
	}
	return console_fd;
}


static void reset_term(int console_fd)
{
	if (console_fd >= 0)
	{
		if (ioctl(console_fd, KDSETMODE, KD_TEXT) < 0) {
			perror("KDSETMODE");
		}
		close(console_fd);
	}
}


struct framebuffer *open_framebuffer()
{
	int fb = 0;
	int console_fd;
	struct fb_var_screeninfo fb_info;
	struct fb_fix_screeninfo fb_finfo;

	console_fd = init_term();

	const char *fb_name = getenv("FRAMEBUFFER");
	if (!fb_name)
		fb_name = "/dev/fb0";

	fb = open(fb_name, O_RDWR);
	if (fb < 0)
	{
		perror("Error: cannot open framebuffer device");
		reset_term(console_fd);
		return NULL;
	}

	// Get variable screen information
	if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_info)) {
		perror("Error reading variable screen info");
		close(fb);
		reset_term(console_fd);
		return NULL;
	}
	if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_finfo)) {
		perror("Error reading fixed screen info.\n");
		close(fb);
		reset_term(console_fd);
		return NULL;
	}
	if (fb_info.bits_per_pixel != 16 &&
		fb_info.bits_per_pixel != 32) {
		fprintf(stderr, "%d bits per pixel is not supported."
			"Only 16 or 32 are handled.\n",
			fb_info.bits_per_pixel);
		close(fb);
		reset_term(console_fd);
		return NULL;
	}
	struct framebuffer *ret = malloc(sizeof(struct framebuffer));

	ret->width = fb_info.xres;
	ret->height = fb_info.yres;
	ret->bpp = fb_info.bits_per_pixel;

	ret->line_length = fb_finfo.line_length;
	ret->data_length = fb_finfo.smem_len;

	ret->u8_data = mmap(NULL, fb_finfo.smem_len, PROT_READ | PROT_WRITE,
		MAP_SHARED, fb, 0);
	ret->console_fd = console_fd;

	if (!ret->u8_data) {
		perror("Cannot map framebuffer");
		close(fb);
		free(ret);
		reset_term(console_fd);
		return NULL;
	}
	ret->fd = fb;

	return ret;
}

void close_framebuffer(struct framebuffer *fb)
{
	close(fb->fd);
	reset_term(fb->console_fd);
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
