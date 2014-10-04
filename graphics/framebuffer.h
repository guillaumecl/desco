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


/**
 * This is just a color in the native system.
 */
struct native_color
{
	uint32_t value;
};

typedef struct native_color color_t;

struct framebuffer *open_framebuffer();

void close_framebuffer(struct framebuffer *fb);

void clear_framebuffer(struct framebuffer *fb, color_t c);

void pause_framebuffer(struct framebuffer *fb);

void resume_framebuffer(struct framebuffer *fb);

void fb_print(struct framebuffer *fb, unsigned int x, unsigned int y, color_t c, color_t back_color, const char *str);
void fb_printf(struct framebuffer *fb, unsigned int x, unsigned int y, color_t c, color_t back_color, const char *format, ...);

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

static inline color_t rgb_to_native(struct framebuffer *fb, uint8_t r, uint8_t g, uint8_t b)
{
	color_t ret;

	if (fb->bpp == 16)
		ret.value = rgb_to_16(r, g, b);
	else
		ret.value = rgb_to_24(r, g, b);

	return ret;
}

static inline int is_transparent(color_t col)
{
	return (col.value & 0x80000000) != 0;
}

#endif
