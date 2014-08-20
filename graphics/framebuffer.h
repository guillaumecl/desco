#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

struct framebuffer
{
	int fd;
	int console_fd;

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

void fb_print(struct framebuffer *fb, unsigned int x, unsigned int y, uint32_t color, uint32_t backcolor, const char *str);
void fb_printf(struct framebuffer *fb, unsigned int x, unsigned int y, uint32_t color, uint32_t backcolor, const char *format, ...);

#define C_16_TO_RGB(c,r,g,b) do { r = ((c>>11)&31)*8; g = ((c>>5)&63)*4; b = (c&31)*8; } while(0)
#define C_32_TO_RGB(c,r,g,b) do { r = (c>>16)&0xff; g = (c>>8)&0xff; b = c&0xff; } while(0)

static inline uint16_t rgb_to_16(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

static inline uint32_t rgb_to_24(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 16) | (g << 8) | b;
}

static inline uint16_t c24_to_16(uint32_t rgb)
{
	return rgb_to_16(rgb >> 16, rgb >> 8, rgb);
}


#endif
