#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

struct input_device;

/**
 * Allocates a touchscreen device using tslib.
 *
 * If path is NULL, use the environment variable TSLIB_TSDEVICE
 */
struct input_device *open_touchscreen(const char *path);

#endif
