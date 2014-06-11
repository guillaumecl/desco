#ifndef FB_PNG_H
#define FB_PNG_H

struct png_file;

struct png_file *open_png_file(char* file_name, struct framebuffer *fb);

void close_png_file(struct png_file *file);


#endif
