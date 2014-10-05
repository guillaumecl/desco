#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include "list.h"

struct input;

/**
 * Contains stuff common to specific classes of devices.
 */
struct input_device_class
{
	/* Device type. */
	const char *name;

	/* Free the device and associated resources. */
	void (*free_device)(struct input_device *);

	/* Read some input from the device. */
	int (*read_device)(struct input_device *, struct input *);
};

/**
 * An input device instance.
 */
struct input_device
{
	struct input_device_class *ops;

	/* Pointer to next/previous device */
	struct list_head entry;

	/* File descriptor to probe using poll */
	int fd;

	/* Private data, depending on the device type */
	void *private_data;
};

#endif
