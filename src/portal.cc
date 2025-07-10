#include "wlkit.h"

#include <stdlib.h>
#include <string.h>

static struct wlkit_portal_callbacks * portal_callbacks = NULL;

static void handle_screencopy_frame(struct wl_listener * listener, void *data) {
	// Handle screencopy frame completion
	(void)listener;
	(void)data;
	// Implementation would handle the completed screenshot
}

static void handle_export_dmabuf_frame(struct wl_listener * listener, void *data) {
	// Handle DMA-buf export for screencasting
	(void)listener;
	(void)data;
	// Implementation would handle the exported frame
}

static void handle_foreign_toplevel_activate(struct wl_listener * listener, void *data) {
	struct wlkit_window *window = wl_container_of(listener, window, request_activate);
	(void)data;

	wlkit_focus_window(window);
}

static void handle_foreign_toplevel_close(struct wl_listener * listener, void *data) {
	struct wlkit_window *window = wl_container_of(listener, window, request_close);
	(void)data;

	wlkit_window_close(window);
}

static void handle_foreign_toplevel_maximize(struct wl_listener * listener, void *data) {
	struct wlkit_window *window = wl_container_of(listener, window, request_maximize);
	(void)data;

	wlkit_window_maximize(window);
}

static void handle_foreign_toplevel_minimize(struct wl_listener * listener, void *data) {
	struct wlkit_window *window = wl_container_of(listener, window, request_minimize);
	(void)data;

	wlkit_window_minimize(window);
}

static void handle_foreign_toplevel_fullscreen(struct wl_listener * listener, void *data) {
	struct wlkit_window *window = wl_container_of(listener, window, request_fullscreen);
	(void)data;

	wlkit_window_fullscreen(window);
}

void wlkit_foreign_toplevel_create(struct wlkit_window * window) {
	if (!window || !window->server || !window->server->portal_manager) {
		return;
	}

	struct wlkit_portal_manager * manager = window->server->portal_manager;

	window->foreign_toplevel = wlr_foreign_toplevel_handle_v1_create(manager->foreign_toplevel_manager);

	if (window->title) {
		wlr_foreign_toplevel_handle_v1_set_title(window->foreign_toplevel, window->title);
	}
	if (window->app_id) {
		wlr_foreign_toplevel_handle_v1_set_app_id(window->foreign_toplevel, window->app_id);
	}

	// Set up event listeners
	window->request_activate.notify = handle_foreign_toplevel_activate;
	wl_signal_add(&window->foreign_toplevel->events.request_activate, &window->request_activate);

	window->request_close.notify = handle_foreign_toplevel_close;
	wl_signal_add(&window->foreign_toplevel->events.request_close, &window->request_close);

	window->request_maximize.notify = handle_foreign_toplevel_maximize;
	wl_signal_add(&window->foreign_toplevel->events.request_maximize, &window->request_maximize);

	window->request_minimize.notify = handle_foreign_toplevel_minimize;
	wl_signal_add(&window->foreign_toplevel->events.request_minimize, &window->request_minimize);

	window->request_fullscreen.notify = handle_foreign_toplevel_fullscreen;
	wl_signal_add(&window->foreign_toplevel->events.request_fullscreen, &window->request_fullscreen);
}

struct wlkit_portal_manager *wlkit_portal_create(struct wlkit_server *server) {
	if (!server) {
		return NULL;
	}

	struct wlkit_portal_manager *manager = calloc(1, sizeof(*manager));
	manager->server = server;
	manager->next_id = 1;

	wl_list_init(&manager->screenshot_requests);
	wl_list_init(&manager->screencast_sessions);

	return manager;
}

void wlkit_portal_destroy(struct wlkit_portal_manager *manager) {
	if (!manager) {
		return;
	}

	// Clean up screenshot requests
	struct wlkit_screenshot_request *req, *tmp_req;
	wl_list_for_each_safe(req, tmp_req, &manager->screenshot_requests, link) {
		free(req);
	}

	// Clean up screencast sessions
	struct wlkit_screencast_session *session, *tmp_session;
	wl_list_for_each_safe(session, tmp_session, &manager->screencast_sessions, link) {
		free(session);
	}

	free(manager);
}

void wlkit_portal_setup_protocols(struct wlkit_portal_manager *manager) {
	if (!manager || !manager->server) {
		return;
	}

	struct wl_display *display = manager->server->wl_display;

	// Create XDG output manager
	manager->xdg_output_manager = wlr_xdg_output_manager_v1_create(display,
		manager->server->output_layout);

	// Create screencopy manager
	manager->screencopy_manager = wlr_screencopy_manager_v1_create(display);

	// Create export DMA-buf manager
	manager->export_dmabuf_manager = wlr_export_dmabuf_manager_v1_create(display);

	// Create gamma control manager
	manager->gamma_manager = wlr_gamma_control_manager_v1_create(display);

	// Create foreign toplevel manager
	manager->foreign_toplevel_manager = wlr_foreign_toplevel_manager_v1_create(display);
}

void wlkit_portal_handle_screenshot(struct wlkit_portal_manager *manager,
								struct wlkit_screenshot_request *request) {
	if (!manager || !request) {
		return;
	}

	if (request->interactive) {
		// Request UI for selection
		if (portal_callbacks) {
			if (request->type == wlkit_SCREENSHOT_WINDOW &&
				portal_callbacks->select_window) {
				request->window = portal_callbacks->select_window();
			} else if (request->type == wlkit_SCREENSHOT_MONITOR &&
					   portal_callbacks->select_output) {
				request->output = portal_callbacks->select_output();
			} else if (request->type == wlkit_SCREENSHOT_REGION &&
					   portal_callbacks->select_region) {
				request->region = portal_callbacks->select_region();
			}
		}
	}

	// Create screencopy frame
	struct wlr_screencopy_frame_v1 *frame =
		wlr_screencopy_frame_v1_create(manager->screencopy_manager);

	if (request->type == wlkit_SCREENSHOT_WINDOW && request->window) {
		// Screenshot specific window
		wlr_screencopy_frame_v1_copy_from_surface(frame, request->window->surface);
	} else if (request->output) {
		// Screenshot output
		wlr_screencopy_frame_v1_copy(frame, request->output);
	}

	// Set up frame completion listener
	struct wl_listener *frame_listener = calloc(1, sizeof(*frame_listener));
	frame_listener->notify = handle_screencopy_frame;
	wl_signal_add(&frame->events.ready, frame_listener);

	if (portal_callbacks && portal_callbacks->on_screenshot_confirmed) {
		portal_callbacks->on_screenshot_confirmed(request);
	}
}

void wlkit_portal_start_screencast(struct wlkit_portal_manager *manager,
							   struct wlkit_screencast_session *session) {
	if (!manager || !session) {
		return;
	}

	// Create DMA-buf export frame
	struct wlr_export_dmabuf_frame_v1 *frame =












	// server->portal_manager->gamma_control_manager_v1 = wlr_gamma_control_manager_v1_create(server->wl_display);
	// wlr_scene_set_gamma_control_manager_v1(server->root->scene, server->portal_manager->gamma_control_manager_v1);

	// server->xdg_shell = wlr_xdg_shell_create(server->wl_display, 1);
	// server->portal_manager->xdg_output_manager_v1 = wlr_xdg_output_manager_v1_create(server->wl_display, server->root->output_layout);

	// wlr_renderer_init_wl_shm(server->renderer, server->wl_display);

	// server->focus_manager = malloc(sizeof(*server->focus_manager));
	// server->focus_manager->server = server;
	// wl_list_init(&server->focus_manager->focus_stack);

	// server->keyboard_manager = malloc(sizeof(*server->keyboard_manager));
	// server->keyboard_manager->server = server;
	// wl_list_init(&server->keyboard_manager->layouts);

	//server->portal_manager = wlkit_portal_create(server);
