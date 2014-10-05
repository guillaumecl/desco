#ifndef TYPES_H
#define TYPES_H

struct list_head {
	struct list_head *next, *prev;
};

struct input;
struct input_device;
struct input_queue;

#endif
