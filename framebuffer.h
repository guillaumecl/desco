#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

struct framebuffer
{
	union
	{
		uint8_t *u8_data;
		uint16_t *u16_data;
		uint32_t *u32_data;
	};

	int width;
	int height;

	int bpp;
};

struct framebuffer *open_framebuffer();

void close_framebuffer(struct framebuffer *fb);

#endif
