#include <errno.h>
#include <tslib.h>
#include <stdlib.h>
#include <memory.h>

#include "touchscreen.h"
#include "input_device.h"
#include "input_event.h"

static inline struct tsdev *to_tsdev(struct input_device *dev)
{
	return (struct tsdev *)dev->private_data;
}

static void close_touchscreen(struct input_device *dev)
{
	if (dev->private_data)
		ts_close(to_tsdev(dev));
	free(dev);
}

static int read_touchscreen(struct input_device *dev, struct input *input)
{
	struct ts_sample samp;
	struct tsdev *ts = to_tsdev(dev);
	int ret = ts_read(ts, &samp, 1);

	if (ret < 0)
		return ret;

	input->type = event_mouse;

	input->mouse.button = mouse_left;
	input->mouse.x = samp.x;
	input->mouse.y = samp.y;

	input->pressed = event_pressed;
	input->device = dev;

	return 0;
}




static struct input_device_class touchscreen_ops = {
	.name = "touchscreen",
	.free_device = close_touchscreen,
	.read_device = read_touchscreen
};



struct input_device *open_touchscreen(const char *path)
{
	if (!path)
		path = getenv("TSLIB_TSDEVICE");

	if (!path) {
		errno = ENODEV;
		return NULL;
	}

	struct input_device *dev = calloc(1, sizeof(struct input_device));
	if (!dev) {
		errno = ENOMEM;
		return NULL;
	}

	struct tsdev *ts;
	ts = ts_open(path, 0);
	if (!ts)
		goto err;

	dev->private_data = ts;

	if (ts_config(ts))
		goto err;

	dev->fd = ts_fd(ts);
	dev->ops = &touchscreen_ops;
	return dev;
err:
	close_touchscreen(dev);
	return NULL;
}
