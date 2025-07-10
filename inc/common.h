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
typedef uint32_t wlkit_screen_request_id;
typedef uint32_t wlkit_node_id;
typedef uint32_t wlkit_geo;

struct wlkit_server;
struct wlkit_output;
struct wlkit_root;
struct wlkit_workspace;
struct wlkit_layout;
struct wlkit_window;
struct wlkit_node;
struct wlkit_cursor;

struct wlkit_keyboard_manager;
struct wlkit_focus_manager;
struct wlkit_portal_manager;

typedef void (*wlkit_notify_handler_t)(struct wl_listener * listener, void * data, struct wlkit_server * server);
struct wlkit_notify_handler {
	struct wl_list link;
	wlkit_notify_handler_t handler;
};

#endif // WLKIT_COMMON_H
