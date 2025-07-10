#include "wlkit.h"

#include <stdlib.h>
#include <string.h>

static void tiling_arrange(struct wlkit_workspace * workspace) {
	if (!workspace || wl_list_empty(&workspace->windows)) {
		return;
	}

	// Simple tiling: divide screen equally among windows
	int window_count = 0;
	struct wlkit_window * window;
	wl_list_for_each(window, &workspace->windows, link) {
		if (window->mapped && !window->minimized) {
			window_count++;
		}
	}

	if (window_count == 0) {
		return;
	}

	// Assume 1920x1080 for now (should get from output)
	int screen_width = 1920;
	int screen_height = 1080;

	if (window_count == 1) {
		// Single window takes full screen
		window = wl_container_of(workspace->windows.next, window, link);
		wlkit_window_move(window, 0, 0);
		wlkit_window_resize(window, screen_width, screen_height);
	} else {
		// Multiple windows: master + stack
		int master_width = screen_width / 2;
		int stack_width = screen_width - master_width;
		int stack_height = screen_height / (window_count - 1);

		int i = 0;
		wl_list_for_each(window, &workspace->windows, link) {
			if (!window->mapped || window->minimized) {
				continue;
			}

			if (i == 0) {
				// Master window
				wlkit_window_move(window, 0, 0);
				wlkit_window_resize(window, master_width, screen_height);
			} else {
				// Stack windows
				wlkit_window_move(window, master_width, (i - 1) * stack_height);
				wlkit_window_resize(window, stack_width, stack_height);
			}
			i++;
		}
	}
}

static void tiling_handle_new_window(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)window;  // unused
	tiling_arrange(workspace);
}

static void tiling_handle_window_removed(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)window;  // unused
	tiling_arrange(workspace);
}

static void tiling_handle_window_focus(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	(void)window;  // unused
	// No special handling needed for tiling
}

static void floating_arrange(struct wlkit_workspace * workspace) {
	(void)workspace;  // floating doesn't auto-arrange
}

static void floating_handle_new_window(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	// Center new window
	wlkit_window_move(window, 100, 100);  // TODO real pos
	wlkit_window_resize(window, 800, 600);  // TODO real size
}

static void floating_handle_window_removed(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	(void)window;  // unused
	// No special handling needed for floating
}

static void floating_handle_window_focus(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	(void)window;  // unused
	// No special handling needed for floating
}

static void fullscreen_arrange(struct wlkit_workspace * workspace) {
	if (!workspace || wl_list_empty(&workspace->windows)) {
		return;
	}

	// All windows are fullscreen
	struct wlkit_window * window;
	wl_list_for_each(window, &workspace->windows, link) {
		if (window->mapped && !window->minimized) {
			wlkit_window_move(window, 0, 0);
			wlkit_window_resize(window, 1920, 1080);  // TODO should get from output
		}
	}
}

static void fullscreen_handle_new_window(struct wlkit_workspace *workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	wlkit_window_move(window, 0, 0);
	wlkit_window_resize(window, 1920, 1080);  // TODO real size
}

static void fullscreen_handle_window_removed(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	(void)window;  // unused
	// No special handling needed
}

static void fullscreen_handle_window_focus(struct wlkit_workspace * workspace, struct wlkit_window * window) {
	(void)workspace;  // unused
	(void)window;  // unused
	// No special handling needed
}

struct wlkit_layout wlkit_layout_tiling = {
	.name = "tiling",
	.arrange = tiling_arrange,
	.handle_new_window = tiling_handle_new_window,
	.handle_window_removed = tiling_handle_window_removed,
	.handle_window_focus = tiling_handle_window_focus,
	.user_data = NULL
};

struct wlkit_layout wlkit_layout_floating = {
	.name = "floating",
	.arrange = floating_arrange,
	.handle_new_window = floating_handle_new_window,
	.handle_window_removed = floating_handle_window_removed,
	.handle_window_focus = floating_handle_window_focus,
	.user_data = NULL
};

struct wlkit_layout wlkit_layout_fullscreen = {
	.name = "fullscreen",
	.arrange = fullscreen_arrange,
	.handle_new_window = fullscreen_handle_new_window,
	.handle_window_removed = fullscreen_handle_window_removed,
	.handle_window_focus = fullscreen_handle_window_focus,
	.user_data = NULL
};

void wlkit_layout_register(struct wlkit_layout * layout) {
	// For now, we only support built-in layouts
	// In the future, this could register custom layouts
	(void) layout;
}

void wlkit_workspace_set_layout(struct wlkit_workspace * workspace, struct wlkit_layout * layout) {
	if (!workspace || !layout) {
		return;
	}

	workspace->layout = layout;
	wlkit_layout_arrange(workspace);
}

void wlkit_layout_arrange(struct wlkit_workspace * workspace) {
	if (!workspace || !workspace->layout || !workspace->layout->arrange) {
		return;
	}

	workspace->layout->arrange(workspace);
}
