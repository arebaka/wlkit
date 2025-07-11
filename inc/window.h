#include "common.h"

#include <wlr/xwayland/xwayland.h>

struct wlkit_window;

typedef void (*wlkit_window_handler_t)(struct wlkit_window * window);
struct wlkit_window_handler {
	struct wl_list link;
	wlkit_window_handler_t handler;
};

typedef void (*wlkit_window_move_handler_t)(struct wlkit_window * window, wlkit_geo_t x, wlkit_geo_t y);
struct wlkit_window_move_handler {
	struct wl_list link;
	wlkit_window_move_handler_t handler;
};

typedef void (*wlkit_window_resize_handler_t)(struct wlkit_window * window, wlkit_geo_t x, wlkit_geo_t y);
struct wlkit_window_resize_handler {
	struct wl_list link;
	wlkit_window_move_handler_t handler;
};

typedef void (*wlkit_window_workspace_changed_handler_t)(struct wlkit_window * window, wlkit_workspace_id old_workspace_id, wlkit_workspace_id new_workspace_id);
struct wlkit_window_workspace_changed_handler {
	struct wl_list link;
	wlkit_window_workspace_changed_handler_t handler;
};

struct wlkit_window {
	struct wl_list link;

	char * title;
	char * app_id;

	wlkit_geo_t x, y, width, height;
	bool mapped, minimized, maximized, fullscreen;

	struct wlkit_server * server;
	struct wlkit_workspace * workspace;

	struct wlr_foreign_toplevel_handle_v1 * foreign_toplevel;
	struct wlr_surface * surface;
	struct wlr_xdg_surface * xdg_surface;
	struct wlr_xwayland_surface * xwayland_surface;

	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener set_title;
	struct wl_listener set_app_id;

	struct wl_listener xwayland_map;
	struct wl_listener xwayland_unmap;
	struct wl_listener xwayland_destroy;
	struct wl_listener xwayland_set_title;
	struct wl_listener xwayland_set_class;

	struct wl_listener request_activate;
	struct wl_listener request_close;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	struct wl_listener request_maximize;
	struct wl_listener request_minimize;
	struct wl_listener request_fullscreen;

	void * user_data;
};

void wlkit_window_destroy(
	struct wlkit_window * window
);

void wlkit_window_move(
	struct wlkit_window * window,
	int x,
	int y
);

void wlkit_window_resize(
	struct wlkit_window * window,
	int width,
	int height
);

void wlkit_window_close(
	struct wlkit_window * window
);

void wlkit_window_maximize(
	struct wlkit_window * window
);

void wlkit_window_minimize(
	struct wlkit_window * window
);

void wlkit_window_fullscreen(
	struct wlkit_window * window
);

void wlkit_window_set_workspace(
	struct wlkit_window * window,
	wlkit_workspace_id workspace_id
);

void wlkit_on_window_new(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_destroy(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_focus(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_unfocus(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_move(struct wlkit_window * window, wlkit_window_move_handler_t handler);
void wlkit_on_window_resize(struct wlkit_window * window, wlkit_window_resize_handler_t handler);
void wlkit_on_window_maximize(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_minimize(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_fullscreen(struct wlkit_window * window, wlkit_window_handler_t handler);
void wlkit_on_window_workspace_changed(struct wlkit_window * window, wlkit_window_workspace_changed_handler_t handler);
