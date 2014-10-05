#include "input.h"
#include "input_device.h"
#include "input_event.h"

#include <poll.h>
#include <stdlib.h>
#include <string.h>

struct input_queue
{
	struct list_head devices;
	struct pollfd *polls;
	int num_devices;
};

void free_input_queue(struct input_queue *queue)
{
	struct input_device *dev;
	list_for_each_entry(dev, &queue->devices, entry) {
		dev->ops->free_device(dev);
	}
	free(queue);
}


struct input_queue *alloc_input_queue()
{
	struct input_queue *queue = calloc(1, sizeof(*queue));
	if (!queue)
		return NULL;

	INIT_LIST_HEAD(&queue->devices);
	queue->num_devices = 0;
	queue->polls = NULL;

	return queue;
}

int register_input(struct input_queue *queue, struct input_device *device)
{
	if (!device)
		return 0;

	struct pollfd *p;
	list_add_tail(&device->entry, &queue->devices);

	p = realloc(queue->polls, queue->num_devices+1);
	if (!p)
		return 1;

	queue->polls = p;

	p = queue->polls + queue->num_devices;
	p->fd = device->fd;
	p->events = POLLIN;
	p->revents = 0;

	++queue->num_devices;

	return 0;
}

void unregister_input(struct input_queue *queue, struct input_device *device)
{
	struct input_device *dev, *n;
	int i = 0;

	list_for_each_entry_safe(dev, n, &queue->devices, entry) {
		if (dev == device) {
			list_del(&dev->entry);
			device->ops->free_device(device);
			queue->polls[i].fd = 0;
			break;
		}
		++i;
	}
	memmove(queue->polls + i, queue->polls + i + 1,
		queue->num_devices - i - 1);
}


int poll_input(struct input_queue *queue, void (*callback)(struct input*, void*),
	int timeout, void *data)
{
	int i = 0;
	int ret;
	struct input_device *dev;

	ret = poll(queue->polls, queue->num_devices, timeout);

	if (ret <= 0)
		return ret;

	list_for_each_entry(dev, &queue->devices, entry) {
		struct input in;
		if (queue->polls[i].revents) {
			if (dev->ops->read_device(dev, &in) == 0)
				callback(&in, data);
		}
		++i;
	}

	return ret;
}
