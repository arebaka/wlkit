#include "wlkit.h"

#include <stdlib.h>

#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>
#include <wlr/types/wlr_keyboard.h>

void wlkit_focus_window(struct wlkit_window * window) {
	if (!window || !window->server) {
		return;
	}

	struct wlkit_focus_manager * focus_manager = window->server->focus_manager;
	if (!focus_manager || !window->workspace) {
		return;
	}

	// Unfocus previous window
	if (focus_manager->focused_window) {
		struct wlkit_workspace * prev_workspace = wlkit_workspace_get(
			window->server,
			focus_manager->focused_window->workspace->id
		);
		if (prev_workspace) {
			prev_workspace->focused_window = NULL;
		}
	}

	// Set new focus
	focus_manager->focused_window = window;
	window->workspace->focused_window = window;

	// Update seat focus
	if (window->surface) {
		wlr_seat_keyboard_notify_enter(window->server->seat, window->surface, NULL, 0, NULL);
	}

	// Update foreign toplevel state
	if (window->foreign_toplevel) {
		wlr_foreign_toplevel_handle_v1_set_activated(window->foreign_toplevel, true);
	}

	// Move to front of focus stack
	wl_list_remove(&window->link);
	wl_list_insert(&focus_manager->focus_stack, &window->link);
}

struct wlkit_window * wlkit_get_focused_window(struct wlkit_server * server) {
	if (!server || !server->focus_manager) {
		return NULL;
	}

	return server->focus_manager->focused_window;
}

void wlkit_focus_next_window(struct wlkit_server * server) {
	if (!server || !server->current_workspace) {
		return;
	}

	struct wlkit_workspace * workspace = server->current_workspace;
	struct wlkit_window * current = workspace->focused_window;

	if (wl_list_empty(&workspace->windows)) {
		return;
	}

	struct wlkit_window * next = NULL;

	if (current) {
		// Find next window in the list
		if (current->link.next != &workspace->windows) {
			next = wl_container_of(current->link.next, next, link);
		} else {
			// Wrap to first window
			next = wl_container_of(workspace->windows.next, next, link);
		}
	} else {
		// No current focus, focus first window
		next = wl_container_of(workspace->windows.next, next, link);
	}

	if (next && next->mapped) {
		wlkit_focus_window(next);
	}
}

void wlkit_focus_prev_window(struct wlkit_server * server) {
	if (!server || !server->current_workspace) {
		return;
	}

	struct wlkit_workspace * workspace = server->current_workspace;
	struct wlkit_window * current = workspace->focused_window;

	if (wl_list_empty(&workspace->windows)) {
		return;
	}

	struct wlkit_window * prev = NULL;

	if (current) {
		// Find previous window in the list
		if (current->link.prev != &workspace->windows) {
			prev = wl_container_of(current->link.prev, prev, link);
		} else {
			// Wrap to last window
			prev = wl_container_of(workspace->windows.prev, prev, link);
		}
	} else {
		// No current focus, focus last window
		prev = wl_container_of(workspace->windows.prev, prev, link);
	}

	if (prev && prev->mapped) {
		wlkit_focus_window(prev);
	}
}
