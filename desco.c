#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>

int init_log()
{
	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	log_file = open("/var/log/desco.log",
		O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);

	if (log_file < 0) {
		perror("Cannot open /var/log/desco.log for writing");

		return 1;
	}

	if (dup2(log_file, fileno(stderr)) != fileno(stderr) ||
		dup2(log_file, fileno(stdout)) != fileno(stdout)) {
		perror("Unable to redirect output");
		return 1;
	}
	return 0;
}


// static void shutdown()
// {
// 	execl("/sbin/shutdown", "shutdown", "-h", "now", NULL);
// }

// static void reboot()
// {
// 	execl("/sbin/shutdown", "shutdown", "-r", "now", NULL);
// }

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	int fb = 0;
	struct fb_var_screeninfo fb_info;

	init_log();

	const char *fb_name = getenv("FRAMEBUFFER");
	if (!fb_name)
		fb_name = "/dev/fb0";

	fb = open(fb_name, O_RDWR);
	if (fb == -1)
	{
		perror("Error: cannot open framebuffer device");
		return 1;
	}

	// Get variable screen information
	if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_info)) {
		printf("Error reading variable screen info.\n");
	}
	printf("Display info %dx%d, %d bpp\n",
		fb_info.xres, fb_info.yres,
		fb_info.bits_per_pixel);

	while(getchar() != EOF)
	{

	}

	// close file
	close(fb);

	return 0;

}
