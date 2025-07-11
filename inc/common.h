#ifndef WLKIT_COMMON_H
#define WLKIT_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <wayland-server-core.h>
#include <wayland-util.h>

#include <wlr/util/box.h>
#include <wlr/util/log.h>

typedef uint32_t wlkit_workspace_id;
typedef uint32_t wlkit_node_id;
typedef uint32_t wlkit_geo_t;
typedef uint32_t wlkit_screen_request_id;

struct wlkit_server;
struct wlkit_output;
struct wlkit_root;
struct wlkit_workspace;
struct wlkit_layout;
struct wlkit_window;
struct wlkit_node;
struct wlkit_cursor;
struct wlkit_render;

struct wlkit_keyboard_manager;
struct wlkit_focus_manager;
struct wlkit_portal_manager;

union wlkit_object {
	struct wlkit_server * server;
	struct wlkit_output * output;
	struct wlkit_root * root;
	struct wlkit_workspace * workspace;
	struct wlkit_layout * layout;
	struct wlkit_window * window;
	struct wlkit_node * node;
	struct wlkit_cursor * cursor;
	struct wlkit_render * render;
};

typedef void (*wlkit_notify_handler_t)(struct wl_listener * listener, void * data, union wlkit_object * object);
struct wlkit_notify_handler {
	struct wl_list link;
	wlkit_notify_handler_t handler;
};

#endif // WLKIT_COMMON_H
