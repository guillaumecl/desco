#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

/* Type of events */
enum input_event_type
{
	event_key = 0,
	event_mouse = 1
};

/* For mouse events, the button types */
enum input_mouse_button
{
	mouse_left = 0,
	mouse_right = 1,
	mouse_middle = 2
};

/* Indicates if the event is a press or release event */
enum input_press_status
{
	event_released = 0,
	event_pressed = 1
};

/* Contains an input event. */
struct input
{
	/* Type of event (mouse, keyboard) */
	enum input_event_type type;

	/* Originating device */
	struct input_device *device;

	union {
		struct {
			/* The pressed button */
			enum input_mouse_button button;

			/* Screen coordinates of the event */
			int x, y;
		} mouse;
		struct {
			/* The pressed key */
			int key;
		} key;
	};

	/* Indicates if the key was pressed or released. */
	enum input_press_status pressed;

	/* Extra data the device may have set. */
	void *extra_data;
};



#endif
