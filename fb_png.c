#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#include "framebuffer.h"

struct png_file {
	int width;
	int height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *data;
};

struct png_file *open_png_file(char* file_name, struct framebuffer *fb)
{
	(void)fb;
	int y;

	int width, height;

	png_structp png_ptr;
	png_infop info_ptr;
	struct png_file * result;
	png_bytep * row_pointers;

        unsigned char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
	{
                perror("Cannot open file");
		return NULL;
	}
        if (fread(header, 1, 8, fp) != 8)
	{
                fprintf(stderr, "%s is not recognized as a PNG file", file_name);
		fclose(fp);
		return NULL;
	}
        if (png_sig_cmp(header, 0, 8))
	{
                fprintf(stderr, "%s is not recognized as a PNG file", file_name);
		fclose(fp);
		return NULL;
	}


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
	{
                fprintf(stderr, "png_create_read_struct failed");
		fclose(fp);
		return NULL;
	}

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
	{
		fprintf(stderr, " png_create_info_struct failed");
		fclose(fp);
		return NULL;
	}

        if (setjmp(png_jmpbuf(png_ptr)))
	{
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

	row_pointers = (png_bytep*) calloc(height, sizeof(png_bytep));

        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
	{
                fprintf(stderr, "Error during read_image");
		fclose(fp);
		for (y = 0; y < height; ++y)
		{
			free(row_pointers[y]);
		}
		free(row_pointers);
		return NULL;
	}

        for (y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);

	result = malloc(sizeof(struct png_file));

        result->width = width;
        result->height = height;
        result->color_type = png_get_color_type(png_ptr, info_ptr);

	// if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        //         abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
	// 		"(lacks the alpha channel)");

        // if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
        //         abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
	// 		PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

	result->bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	result->data = row_pointers;

	return result;
}

void blit(struct png_file *file, struct framebuffer *fb)
{
	(void) file;
	(void) fb;
        // for (y=0; y<file->height; y++) {
        //         png_byte* row = row_pointers[y];
        //         for (x=0; x<file->width; x++) {
        //                 png_byte* ptr = &(row[x*4]);
        //                 printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
        //                        x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

        //                 /* set red value to 0 and green value to the blue one */
        //                 ptr[0] = 0;
        //                 ptr[1] = ptr[2];
        //         }
        // }

}
