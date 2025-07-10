#include "wlkit.h"
#include <wayland-util.h>

struct wlkit_workspace * wlkit_workspace_create(
	struct wlkit_server * server,
	struct wlkit_layout * layout,
	wlkit_workspace_id id,
	const char * name
) {
    if (!server) {
        return NULL;
    }

    struct wlkit_workspace * workspace = malloc(sizeof(*workspace));
    if (!workspace) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit workspace");
        return NULL;
    }

    workspace->server = server;
    workspace->layout = layout;
    workspace->id = id;
    workspace->name = strdup(name ? name : "");
	workspace->focused_window = NULL;

    wl_list_init(&workspace->windows);

	wl_list_init(&workspace->handlers.create);
	wl_list_init(&workspace->handlers.destroy);
	wl_list_init(&workspace->handlers.activate);
	wl_list_init(&workspace->handlers.deactivate);
	wl_list_init(&workspace->handlers.window_added);
	wl_list_init(&workspace->handlers.window_removed);
	wl_list_init(&workspace->handlers.layout_change);

    wl_list_insert(&server->workspaces, &workspace->link);

	struct wlkit_workspace_handler * wrapper;
	wl_list_for_each(wrapper, &workspace->handlers.create, link) {
		wrapper->handler(workspace);
	}

    return workspace;
}

void wlkit_workspace_destroy(struct wlkit_workspace * workspace, struct wlkit_workspace * reserved_workspace) {
    if (!workspace) {
        return;
    }

	struct wlkit_workspace_handler * wrapper;
	wl_list_for_each(wrapper, &workspace->handlers.destroy, link) {
		wrapper->handler(workspace);
	}

    // Move all windows to reserved workspace (or destroy them)
    struct wlkit_window * window, * tmp;
    wl_list_for_each_safe(window, tmp, &workspace->windows, link) {
        if (reserved_workspace && workspace->id != reserved_workspace->id) {
            wlkit_window_set_workspace(window, reserved_workspace->id);
        } else {
            wlkit_window_close(window);
        }
    }

    wl_list_remove(&workspace->windows);
    free(workspace->name);
    free(workspace);
}

void wlkit_workspace_switch(struct wlkit_server * server, wlkit_workspace_id workspace_id) {
    if (!server) {
        return;
    }

    struct wlkit_workspace * workspace = wlkit_workspace_get(server, workspace_id);
    if (!workspace || workspace == server->current_workspace) {
        return;
    }

    // Hide current workspace windows
    if (server->current_workspace) {
        struct wlkit_window * window;
        wl_list_for_each(window, &server->current_workspace->windows, link) {
            if (window->mapped) {
                wlr_surface_send_leave(window->surface, NULL);  // TODO via window
            }
        }
    }

    server->current_workspace = workspace;

    // Show new workspace windows
    struct wlkit_window * window;
    wl_list_for_each(window, &workspace->windows, link) {
        if (window->mapped) {
            wlr_surface_send_enter(window->surface, NULL);  // TODO via window
        }
    }

    // Focus first window in new workspace
    if (!wl_list_empty(&workspace->windows)) {
        struct wlkit_window *first_window = wl_container_of(workspace->windows.next, first_window, link);
        if (first_window->mapped) {
            wlkit_focus_window(first_window);
        }
    }

    wlkit_layout_arrange(workspace);
}

struct wlkit_workspace * wlkit_workspace_get(struct wlkit_server * server, uint32_t workspace_id) {
    if (!server) {
        return NULL;
    }

    struct wlkit_workspace * workspace;
    wl_list_for_each(workspace, &server->workspaces, link) {
        if (workspace->id == workspace_id) {
            return workspace;
        }
    }

    return NULL;
}
