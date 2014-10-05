#ifndef INPUT_H
#define INPUT_H

struct input;
struct input_device;
struct input_queue;

/* Initializes an input queue. */
struct input_queue *alloc_input_queue();

/* Free the input queue and all registered devices. */
void free_input_queue(struct input_queue *device);

/* Register an input driver to be polled. */
int register_input(struct input_queue *queue, struct input_device *device);

/* Poll all the drivers to see if they have new data. */
int poll_input(struct input_queue *queue, void (*callback)(struct input*, void*),
	int timeout, void *data);

/* Unregister some input driver. */
void unregister_input(struct input_queue *queue, struct input_device *device);

#endif
