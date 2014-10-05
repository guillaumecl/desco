#ifndef STDIN_H
#define STDIN_H

struct input_device;

/**
 * Allocates a keyboard device using stdin.
 */
struct input_device *open_stdin();


#endif
