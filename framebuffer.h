#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

struct framebuffer
{
	int fd;
	int virtual_terminal;

	union
	{
		uint8_t *u8_data;
		uint16_t *u16_data;
		uint32_t *u32_data;
	};

	int width;
	int height;

	int bpp;
	unsigned int data_length;
	unsigned int line_length;
};

struct framebuffer *open_framebuffer();

void close_framebuffer(struct framebuffer *fb);

void clear_framebuffer(struct framebuffer *fb, uint8_t r, uint8_t g, uint8_t b);

#define C_24_TO_16(rgb)  (((((rgb>>16)&0xff) / 8) << 11) | ((((rgb>>8)&0xff) / 4) << 5) | (rgb / 8))

#define C_RGB_TO_16(r,g,b) ((((r) / 8) << 11) | ((g / 4) << 5) | (b / 8))
#define C_RGB_TO_24(r,g,b) ((r<<16) | (g << 8) | b  )


#endif
