#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stropts.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>

#include "framebuffer.h"

int restore_term;

int quit;

static void interrupt_desco(int signal)
{
	(void)signal;

	quit = 1;
}

static int init_term()
{
	int log_file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	restore_term = 0;
	if (isatty(STDOUT_FILENO))
	{
		if (!getenv("VT"))
			return 0;
		restore_term = 1;
		log_file = open("/tmp/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);

		if (system("setterm -cursor off"))
		{}
		struct termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
	else
	{
		log_file = open("/var/log/desco.log",
			O_WRONLY | O_CREAT | O_APPEND | O_SYNC, mode);
		int fd = open("/sys/class/graphics/fbcon/cursor_blink", O_WRONLY|O_TRUNC);
		if (fd >= 0)
		{
			while (write(fd, "0", 1) == EINTR)
			{
			}

			close(fd);
		}
	}
	if (log_file < 0) {
		perror("Cannot open log file for writing");

		return 1;
	}

	if (dup2(log_file, STDERR_FILENO) != STDERR_FILENO ||
		dup2(log_file, STDOUT_FILENO) != STDOUT_FILENO) {
		perror("Unable to redirect output");
		return 1;
	}
	return 0;
}

static void reset_term()
{
	if (restore_term)
	{
		struct termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag |= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);

		if (system("setterm -cursor on"))
		{
			perror("setterm failed");
		}
	}
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

	quit = 0;

	signal(SIGINT, interrupt_desco);
	signal(SIGTERM, interrupt_desco);

	init_term();

	struct framebuffer *fb = open_framebuffer();

	clear_framebuffer(fb, 0, 127, 0);

	if (isatty(STDIN_FILENO))
	{
		while(getchar() != EOF && !quit)
		{

		}
	}
	else
	{
		while(!quit)
		{
			sleep(10);
		}
	}

	fprintf(stderr, "No more input, terminating.\n");

	clear_framebuffer(fb, 0, 0, 0);

	// close file
	close_framebuffer(fb);
	reset_term();

	return 0;

}
