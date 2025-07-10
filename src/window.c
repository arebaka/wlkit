#include "wlkit.h"

#include <wlr/types/wlr_xdg_shell.h>

void wlkit_window_destroy(struct wlkit_window * window) {
	free(window->title);
	free(window->app_id);
	free(window->user_data);
	free(window);
}

void wlkit_window_move(struct wlkit_window * window, int x, int y) {
	if (!window) {
		return;
	}

	window->x = x;
	window->y = y;

	if (window->xdg_surface && window->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_size((struct wlr_xdg_toplevel *) window->xdg_surface, window->width, window->height);
	}
}

void wlkit_window_resize(struct wlkit_window * window, int width, int height) {
	if (!window) {
		return;
	}

	window->width = width;
	window->height = height;

	if (window->xdg_surface && window->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_size((struct wlr_xdg_toplevel *) window->xdg_surface, width, height);
	}
}

void wlkit_window_close(struct wlkit_window * window) {
	if (!window || !window->xdg_surface) {
		return;
	}

	if (window->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_send_close((struct wlr_xdg_toplevel *) window->xdg_surface);
	}
}

void wlkit_window_maximize(struct wlkit_window * window) {
	if (!window || !window->xdg_surface) {
		return;
	}

	window->maximized = !window->maximized;

	if (window->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_maximized((struct wlr_xdg_toplevel *) window->xdg_surface, window->maximized);
	}

	// Update foreign toplevel state
	if (window->foreign_toplevel) {
		if (window->maximized) {
			wlr_foreign_toplevel_handle_v1_set_maximized(window->foreign_toplevel, true);
		} else {
			wlr_foreign_toplevel_handle_v1_set_maximized(window->foreign_toplevel, false);
		}
	}
}

void wlkit_window_minimize(struct wlkit_window * window) {
	if (!window || !window->xdg_surface) {
		return;
	}

	window->minimized = !window->minimized;

	// Update foreign toplevel state
	if (window->foreign_toplevel) {
		if (window->minimized) {
			wlr_foreign_toplevel_handle_v1_set_minimized(window->foreign_toplevel, true);
		} else {
			wlr_foreign_toplevel_handle_v1_set_minimized(window->foreign_toplevel, false);
		}
	}
}

void wlkit_window_fullscreen(struct wlkit_window * window) {
	if (!window || !window->xdg_surface) {
		return;
	}

	window->fullscreen = !window->fullscreen;

	if (window->xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_fullscreen((struct wlr_xdg_toplevel *) window->xdg_surface, window->fullscreen);
	}

	// Update foreign toplevel state
	if (window->foreign_toplevel) {
		if (window->fullscreen) {
			wlr_foreign_toplevel_handle_v1_set_fullscreen(window->foreign_toplevel, true);
		} else {
			wlr_foreign_toplevel_handle_v1_set_fullscreen(window->foreign_toplevel, false);
		}
	}
}

void wlkit_window_set_workspace(struct wlkit_window * window, wlkit_workspace_id workspace_id) {
	if (!window || !window->server) {
		return;
	}

	struct wlkit_workspace * new_workspace = wlkit_workspace_get(window->server, workspace_id);
	if (!new_workspace || new_workspace->id == window->workspace->id) {
		return;
	}

	wl_list_remove(&window->link);
	wl_list_insert(&new_workspace->windows, &window->link);

	// If this was the focused window, update focus
	struct wlkit_workspace * old_workspace = wlkit_workspace_get(window->server, window->workspace->id);
	if (old_workspace && old_workspace->focused_window == window) {
		old_workspace->focused_window = NULL;
		wlkit_focus_next_window(window->server);
	}

	window->workspace = new_workspace;
}

struct wlkit_window * wlkit_window_at(struct wlkit_server * server, double x, double y) {
	if (!server) {
		return NULL;
	}

	struct wlkit_window * window;
	wl_list_for_each(window, &server->current_workspace->windows, link) {
		if (!window->mapped) {
			continue;
		}

		if (x >= window->x && x < window->x + window->width &&
			y >= window->y && y < window->y + window->height) {
			return window;
		}
	}

	return NULL;
}
