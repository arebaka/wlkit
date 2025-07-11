#include "common.h"

#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

typedef uint32_t wlkit_cursor_size;

struct wlkit_cursor {
	struct wl_list link;
	struct wlr_cursor * cursor;
	struct wlr_xcursor_manager * xcursor_manager;

	struct {
		struct wl_listener motion;
		struct wl_listener motion_absolute;
		struct wl_listener button;
		struct wl_listener axis;
		struct wl_listener frame;
	} listeners;

	void * user_data;
};

struct wlkit_cursor * wlkit_cursor_create(
	struct wlkit_root * root,
	char * name, wlkit_cursor_size size
);
