#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "stdin.h"
#include "input_device.h"
#include "input_event.h"

static void close_stdin(struct input_device *dev)
{
	free(dev);
}

static int read_stdin(struct input_device *dev, struct input *input)
{
	int c = 0;
	int ret = read(dev->fd, &c, 1);

	if (ret < 0)
		return ret;

	input->type = event_key;

	input->key.key = c;

	input->pressed = event_pressed;
	input->device = dev;

	return 0;
}




static struct input_device_class stdin_ops = {
	.name = "stdin",
	.free_device = close_stdin,
	.read_device = read_stdin
};



struct input_device *open_stdin()
{
	struct input_device *dev = calloc(1, sizeof(struct input_device));
	if (!dev) {
		errno = ENOMEM;
		return NULL;
	}

	dev->fd = STDIN_FILENO;
	dev->ops = &stdin_ops;
	return dev;
}
