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

	unsigned int width;
	unsigned int height;

	unsigned int bpp;
	unsigned int data_length;
	unsigned int line_length;
};

struct framebuffer *open_framebuffer();

void close_framebuffer(struct framebuffer *fb);

void clear_framebuffer(struct framebuffer *fb, uint8_t r, uint8_t g, uint8_t b);


#define C_RGB_TO_16(r,g,b) ((((r) / 8) << 11) | ((g / 4) << 5) | (b / 8))
#define C_RGB_TO_24(r,g,b) ((r<<16) | (g << 8) | b  )

#define C_24_TO_16(rgb)  C_RGB_TO_16( ((rgb>>16)&0xff), ((rgb>>8)&0xff), (rgb&0xff))

#define C_16_TO_24(c,r,g,b) do { r = ((c>>11)&31)*8; g = ((c>>5)&63)*4; b = (c&31)*8; } while(0)


#endif
