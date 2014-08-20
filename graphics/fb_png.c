#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#include "framebuffer.h"

struct png_file {
	unsigned int width;
	unsigned int height;

	union {
		uint8_t *data;
		uint16_t *u16_data;
		uint32_t *u32_data;
	};
	uint8_t *alpha;
};

struct png_file *open_png(char* file_name, struct framebuffer *fb)
{
	(void)fb;
	int x,y;

	int width, height;

	png_structp png_ptr;
	png_infop info_ptr;
	struct png_file * result;
	png_bytep row;

	uint8_t *data;
	uint8_t *alpha;

        unsigned char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp) {
                perror("Cannot open file");
		return NULL;
	}
        if (fread(header, 1, 8, fp) != 8) {
                fprintf(stderr, "%s is not recognized as a PNG file", file_name);
		fclose(fp);
		return NULL;
	}
        if (png_sig_cmp(header, 0, 8)) {
                fprintf(stderr, "%s is not recognized as a PNG file", file_name);
		fclose(fp);
		return NULL;
	}


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr) {
                fprintf(stderr, "png_create_read_struct failed");
		fclose(fp);
		return NULL;
	}

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
		fprintf(stderr, " png_create_info_struct failed");
		fclose(fp);
		return NULL;
	}

        if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Cannot read the PNG file");
		fclose(fp);
		return NULL;
	}

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);

        png_read_update_info(png_ptr, info_ptr);

	row = (png_bytep) malloc(png_get_rowbytes(png_ptr,info_ptr));
	data = malloc(width * height * fb->bpp / 8);
	alpha = malloc(width * height);

        /* read file */
        if (setjmp(png_jmpbuf(png_ptr))) {
                fprintf(stderr, "Error during read_image");
		fclose(fp);
		free(row);
		return NULL;
	}

        for (y=0; y<height; y++) {
		png_read_row(png_ptr, row, NULL);

		uint8_t *row_data = data + (y * width) * fb->bpp / 8;
		if (fb->bpp == 16) {
			for (x=0; x < width ; x++) {
				png_byte* ptr = &(row[x*4]);
				((uint16_t*)row_data)[x] = rgb_to_16(ptr[0], ptr[1], ptr[2]);
			}
		} else {
			for (x=0; x < width ; x++) {
				png_byte* ptr = &(row[x*4]);
				((uint32_t*)row_data)[x] = rgb_to_24(ptr[0], ptr[1], ptr[2]);
			}
		}

		for (x=0; x < width ; x++) {
			png_byte* ptr = &(row[x*4]);
			alpha[y * width + x] = ptr[3];
		}
	}

        fclose(fp);

	result = malloc(sizeof(struct png_file));

        result->width = width;
        result->height = height;
        //int color_type = png_get_color_type(png_ptr, info_ptr);

	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB) {
                fprintf(stderr, "[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
			"(lacks the alpha channel)");
		exit(1);
	}

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA) {
                fprintf(stderr, "[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
			PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
		exit(1);
	}

	result->data = data;
	result->alpha = alpha;

	return result;
}

void close_png(struct png_file *file)
{
	free(file);
}

void blit_png(struct png_file *image, struct framebuffer *fb,
	unsigned int dst_x, unsigned int dst_y)
{
	(void) image;
	(void) fb;

	if (image->width == fb->width) {
		// TODO bound checking.
		memcpy(fb->u8_data, image->data,
			(image->width * image->height + dst_x) * fb->bpp / 8);
		return;
	}
	int y;
	int max_y;
	if (dst_y + image->height < fb->height)
		max_y = image->height;
	else
		max_y = fb->height - dst_y;


	for (y = 0; y < max_y; ++y) {
		// TODO bound checking for x.
		memcpy(fb->u8_data + ((y+dst_y) * fb->line_length + dst_x * fb->bpp / 8),
			image->data + (y * image->width * fb->bpp / 8),
			image->width * fb->bpp / 8);
	}
}

void alpha_blit_png(struct png_file *image, struct framebuffer *fb,
	unsigned int dst_x, unsigned int dst_y)
{
	int x, y;
	int max_x, max_y;
	if (dst_y + image->height < fb->height)
		max_y = image->height;
	else
		max_y = fb->height - dst_y;

	if (dst_x + image->width < fb->width)
		max_x = image->width;
	else
		max_x = fb->width - dst_x;


	for (y = 0; y < max_y; ++y) {
		for(x = 0; x < max_x ; ++x) {
			uint8_t alpha = 255-image->alpha[y*image->width + x];
			if (alpha == 255)
				continue;
			if (alpha == 0) {
				memcpy(fb->u8_data + ((y+dst_y) * fb->line_length + (dst_x+x) * fb->bpp / 8),
					image->data + ((y * image->width+x) * fb->bpp / 8),
					fb->bpp / 8);
			} else {
				if (fb->bpp == 16) {
					uint16_t src = image->u16_data[y * image->width + x];
					uint16_t dst = fb->u16_data[(y+dst_y) * fb->width + dst_x + x];

					uint16_t sr, sg, sb;
					uint16_t dr, dg, db;
					uint16_t rr, rg, rb;

					C_16_TO_RGB(src, sr, sg, sb);
					C_16_TO_RGB(dst, dr, dg, db);

					rr = (dr * alpha + sr * (255 - alpha)) >> 8;
					rg = (dg * alpha + sg * (255 - alpha)) >> 8;
					rb = (db * alpha + sb * (255 - alpha)) >> 8;

					fb->u16_data[(y+dst_y) * fb->width + dst_x + x] =
						rgb_to_16(rr, rg, rb);
				} else {
					uint32_t src = image->u32_data[y * image->width + x];
					uint32_t dst = fb->u32_data[(y+dst_y) * fb->width + dst_x + x];

					uint16_t sr, sg, sb;
					uint16_t dr, dg, db;
					uint16_t rr, rg, rb;

					C_32_TO_RGB(src, sr, sg, sb);
					C_32_TO_RGB(dst, dr, dg, db);

					rr = (dr * alpha + sr * (255 - alpha)) >> 8;
					rg = (dg * alpha + sg * (255 - alpha)) >> 8;
					rb = (db * alpha + sb * (255 - alpha)) >> 8;

					fb->u32_data[(y+dst_y) * fb->width + dst_x + x] =
						rgb_to_24(rr, rg, rb);
				}
			}

		}
	}
}
