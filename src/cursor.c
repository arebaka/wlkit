#include "wlkit.h"

static void handle_motion(struct wl_listener * listener, void * data) {};
static void handle_motion_absolute(struct wl_listener * listener, void * data) {};
static void handle_button(struct wl_listener * listener, void * data) {};
static void handle_axis(struct wl_listener * listener, void * data) {};
static void handle_frame(struct wl_listener * listener, void * data) {};

struct wlkit_cursor * wlkit_cursor_create(struct wlkit_root * root, char * name, wlkit_cursor_size size) {
	if (!root) {
		return NULL;
	}

	struct wlkit_cursor * cursor = malloc(sizeof(*cursor));
	if (!cursor) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit cursor");
		return NULL;
	}

	cursor->cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(cursor->cursor, root->output_layout);

	cursor->xcursor_manager = wlr_xcursor_manager_create(name, size);
	wlr_xcursor_manager_load(cursor->xcursor_manager, 1);

	cursor->listeners.motion.notify = handle_motion;
	wl_signal_add(&cursor->cursor->events.motion, &cursor->listeners.motion);
	cursor->listeners.motion_absolute.notify = handle_motion_absolute;
	wl_signal_add(&cursor->cursor->events.motion_absolute, &cursor->listeners.motion_absolute);
	cursor->listeners.button.notify = handle_button;
	wl_signal_add(&cursor->cursor->events.button, &cursor->listeners.button);
	cursor->listeners.axis.notify = handle_axis;
	wl_signal_add(&cursor->cursor->events.axis, &cursor->listeners.axis);
	cursor->listeners.frame.notify = handle_frame;
	wl_signal_add(&cursor->cursor->events.frame, &cursor->listeners.frame);

	cursor->user_data = NULL;

	return cursor;
}
