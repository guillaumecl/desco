#include "framebuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>
#include <termios.h>
#include <errno.h>


static int init_term()
{
	int virtual_terminal = 0;
	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if (isatty(STDOUT_FILENO))
	{
		if (!getenv("VT"))
			return 0;
		virtual_terminal = 1;
		log_file = open("/tmp/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);

		if (system("setterm -cursor off"))
		{}
		struct termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
	else
	{
		log_file = open("/var/log/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);
		int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY|O_TRUNC);
		if (fd >= 0)
		{
			while (write(fd, "0", 1) == EINTR)
			{
			}

			close(fd);
		}
	}
	if (log_file < 0) {
		perror("Cannot open log file for writing");

		return 0;
	}

	if (dup2(log_file, STDERR_FILENO) != STDERR_FILENO ||
		dup2(log_file, STDOUT_FILENO) != STDOUT_FILENO) {
		perror("Unable to redirect output");
		return 0;
	}
	return virtual_terminal;
}


static void reset_term(int virtual_terminal)
{
	if (virtual_terminal)
	{
		struct termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag |= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);

		if (system("setterm -cursor on"))
		{
			perror("setterm failed");
		}
	}
}


struct framebuffer *open_framebuffer()
{
	int fb = 0;
	int virtual_terminal;
	struct fb_var_screeninfo fb_info;
	struct fb_fix_screeninfo fb_finfo;

	virtual_terminal = init_term();

	const char *fb_name = getenv("FRAMEBUFFER");
	if (!fb_name)
		fb_name = "/dev/fb0";

	fb = open(fb_name, O_RDWR);
	if (fb < 0)
	{
		perror("Error: cannot open framebuffer device");
		reset_term(virtual_terminal);
		return NULL;
	}

	// Get variable screen information
	if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_info)) {
		perror("Error reading variable screen info");
		close(fb);
		reset_term(virtual_terminal);
		return NULL;
	}
	if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_finfo)) {
		perror("Error reading fixed screen info.\n");
		close(fb);
		reset_term(virtual_terminal);
		return NULL;
	}
	if (fb_info.bits_per_pixel != 16 &&
		fb_info.bits_per_pixel != 32) {
		fprintf(stderr, "%d bits per pixel is not supported."
			"Only 16 or 32 are handled.\n",
			fb_info.bits_per_pixel);
		close(fb);
		reset_term(virtual_terminal);
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
	ret->virtual_terminal = virtual_terminal;

	if (!ret->u8_data) {
		perror("Cannot map framebuffer");
		close(fb);
		free(ret);
		reset_term(virtual_terminal);
		return NULL;
	}
	ret->fd = fb;

	return ret;
}

void close_framebuffer(struct framebuffer *fb)
{
	close(fb->fd);
	reset_term(fb->virtual_terminal);
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
