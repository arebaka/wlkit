#ifndef WLKIT_WORKSPACE_H
#define WLKIT_WORKSPACE_H

#include "common.h"

struct wlkit_workspace;
typedef void (*wlkit_workspace_handler_t)(struct wlkit_workspace * workspace);
struct wlkit_workspace_handler {
	struct wl_list link;
	wlkit_workspace_handler_t handler;
};

struct wlkit_workspace {
	struct wl_list link;

	wlkit_workspace_id id;
	char * name;

	struct wl_list windows;

	struct wlkit_server * server;
	struct wlkit_layout * layout;
	struct wlkit_window * focused_window;

	struct {
		struct wl_list create;
		struct wl_list destroy;
		struct wl_list activate;
		struct wl_list deactivate;
		struct wl_list window_added;
		struct wl_list window_removed;
		struct wl_list layout_change;
	} handlers;

	void * user_data;
};

struct wlkit_workspace * wlkit_workspace_create(
	struct wlkit_server * server,
	struct wlkit_layout * layout,
	wlkit_workspace_id id,
	const char * name
);

void wlkit_workspace_destroy(
	struct wlkit_workspace * workspace,
	struct wlkit_workspace * reserved_workspace
);

void wlkit_workspace_switch(
	struct wlkit_server * server,
	wlkit_workspace_id workspace_id
);

struct wlkit_workspace * wlkit_workspace_get(
	struct wlkit_server * server,
	wlkit_workspace_id workspace_id
);

void wlkit_on_workspace_create(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_destroy(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_activate(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_deactivate(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_window_added(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_window_removed(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

void wlkit_on_workspace_layout_change(
	struct wlkit_workspace * workspace,
	wlkit_workspace_handler_t handler
);

#endif // WLKIT_WORKSPACE_H
