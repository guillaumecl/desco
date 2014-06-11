#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>

#include "framebuffer.h"

static int init_log()
{
	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	log_file = open("/var/log/desco.log",
		O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);

	if (log_file < 0) {
		perror("Cannot open /var/log/desco.log for writing");

		return 1;
	}

	if (dup2(log_file, STDERR_FILENO) != STDERR_FILENO ||
		dup2(log_file, STDOUT_FILENO) != STDOUT_FILENO) {
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

	if (!isatty(STDOUT_FILENO) || !isatty(STDERR_FILENO))
	{
		init_log();
	}

	struct framebuffer *fb = open_framebuffer();

	clear_framebuffer(fb, 0, 127, 0);

	if (isatty(STDIN_FILENO))
	{
		while(getchar() != EOF)
		{

		}
	}
	else
	{
		sleep(10);
	}

	fprintf(stderr, "No more input, terminating.\n");

	clear_framebuffer(fb, 0, 0, 0);

	// close file
	close_framebuffer(fb);

	return 0;

}
