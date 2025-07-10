#ifndef WLKIT_WORKSPACE_H
#define WLKIT_WORKSPACE_H

#include "common.h"

struct wlkit_workspace {
	struct wl_list link;

	wlkit_workspace_id id;
	char * name;

	struct wl_list windows;

	struct wlkit_server * server;
	struct wlkit_layout * layout;
	struct wlkit_window * focused_window;

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

#endif // WLKIT_WORKSPACE_H
