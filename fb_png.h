#ifndef FB_PNG_H
#define FB_PNG_H

struct png_file;

struct png_file *open_png(char* file_name, struct framebuffer *fb);

void blit_png(struct png_file *file, struct framebuffer *fb, unsigned int x, unsigned int y);

void close_png(struct png_file *file);



#endif
