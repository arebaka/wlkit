#include "wlkit.h"

#include <wayland-server-core.h>
#include <wayland-util.h>
#include <wlr/types/wlr_linux_drm_syncobj_v1.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_subcompositor.h>

static void handle_new_output(struct wl_listener * listener, void * data) {
	struct wlkit_server * server = wl_container_of(listener, server, listeners.new_output);
	struct wlr_output * wlr_output = data;

	struct wlkit_output * output = wlkit_output_create(server, wlr_output);
	if (!output) {
		wlr_log(WLR_ERROR, "Failed to create a wlkit output");
		return;
	}

	union wlkit_object object = { .output = output };
	struct wlkit_notify_handler * wrapper;
	wl_list_for_each(wrapper, &server->handlers.new_output, link) {
		wrapper->handler(listener, data, &object);
	}

}

static void handle_new_input(struct wl_listener * listener, void * data) {
	struct wlkit_server * server = wl_container_of(listener, server, listeners.new_input);
	struct wlr_input_device * device = data;

	struct wlkit_input * input = malloc(sizeof(*input));
	if (!input) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit input");
		return;
	}
	input->server = server;
	input->device = device;

	switch (device->type) {
	case WLR_INPUT_DEVICE_KEYBOARD:
		// wlr_seat_set_keyboard(server->seat, device);  // TODO create keyboard
		break;
	case WLR_INPUT_DEVICE_POINTER:
		// wlr_seat_set_pointer(server->seat, device);  // TODO create pointer
		break;
	default:
		break;
	}

	wlr_seat_set_capabilities(server->seat,
		WL_SEAT_CAPABILITY_KEYBOARD | WL_SEAT_CAPABILITY_POINTER);

	wl_list_insert(&server->inputs, &input->link);
}

static void handle_new_xdg_surface(struct wl_listener * listener, void * data) {
	struct wlkit_server * server = wl_container_of(listener, server, listeners.new_xdg_surface);
	struct wlr_xdg_surface * xdg_surface = data;

	if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		return;
	}

	struct wlkit_window * window = malloc(sizeof(*window));
	if (!window) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit window");
		return;
	}

	window->server = server;
	window->xdg_surface = xdg_surface;
	window->surface = xdg_surface->surface;
	window->workspace = server->current_workspace;

	// Add to current workspace
	wl_list_insert(&server->current_workspace->windows, &window->link);
	wl_list_insert(&server->windows, &window->link);

	// Set up listeners
	// Will be implemented in window.c
	window->map.notify = NULL;
	window->unmap.notify = NULL;
	window->destroy.notify = NULL;

	// Create foreign toplevel for portal support
	if (server->portal_manager) {
		window->foreign_toplevel = wlr_foreign_toplevel_handle_v1_create(
			server->portal_manager->foreign_toplevel_manager_v1);
	}
}

static void handle_xdg_activation_v1_request_activate(struct wl_listener * listener, void * data) {}

static void handle_xdg_activation_v1_new_token(struct wl_listener * listener, void * data) {}

static void handle_xdg_shell_toplevel(struct wl_listener * listener, void * data) {}

static void handle_new_foreign_toplevel_capture_request(struct wl_listener * listener, void * data) {}

static void handle_new_server_decoration(struct wl_listener * listener, void * data) {}

static void handle_new_xdg_decoration(struct wl_listener * listener, void * data) {}

struct wlkit_server * wlkit_create(struct wl_display * display, struct wlr_seat * seat, wlkit_server_handler_t callback) {
	if (!display || !seat) {
		return NULL;
	}

	struct wlkit_server * server = malloc(sizeof(*server));
	if (!server) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit server");
		return NULL;
	}

	server->display = display;
	server->seat = seat;
	server->running = false;
	server->user_data = NULL;

	wl_list_init(&server->handlers.create);
	wl_list_init(&server->handlers.destroy);
	wl_list_init(&server->handlers.start);
	wl_list_init(&server->handlers.stop);

	if (callback) {
		struct wlkit_server_handler * wrapper = malloc(sizeof(*wrapper));
		wrapper->handler = callback;
		wl_list_insert(&server->handlers.create, &wrapper->link);
	}

	server->event_loop = wl_display_get_event_loop(server->display);

	server->backend = wlr_backend_autocreate(server->event_loop, &server->session);
	if (!server->backend) {
		wlr_log(WLR_ERROR, "Unable to create wlkit backend");
		wl_event_loop_destroy(server->event_loop);
		free(server);
		return NULL;
	}

	server->renderer = wlr_renderer_autocreate(server->backend);
	if (!server->renderer) {
		wlr_log(WLR_ERROR, "Unable to create wlkit renderer");
		wlr_backend_destroy(server->backend);
		wl_event_loop_destroy(server->event_loop);
		free(server);
		return NULL;
	}

	wlr_renderer_init_wl_display(server->renderer, server->display);

	server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);
	if (!server->allocator) {
		wlr_log(WLR_ERROR, "Unable to create wlkit allocator");
		wlr_renderer_destroy(server->renderer);
		wlr_backend_destroy(server->backend);
		wl_event_loop_destroy(server->event_loop);
		free(server);
		return NULL;
	}

	server->compositor = wlr_compositor_create(server->display, 6, server->renderer);
	wlr_subcompositor_create(server->display);

	server->data_device_manager = wlr_data_device_manager_create(server->display);
	server->idle_notifier_v1 = wlr_idle_notifier_v1_create(server->display);

	if (wlr_renderer_get_texture_formats(server->renderer, WLR_BUFFER_CAP_DMABUF) != NULL) {
		server->linux_dmabuf_v1 = wlr_linux_dmabuf_v1_create_with_renderer(server->display, 4, server->renderer);
	}
	if (wlr_renderer_get_drm_fd(server->renderer) >= 0 &&
		server->renderer->features.timeline &&
		server->backend->features.timeline
	) {
		wlr_linux_drm_syncobj_manager_v1_create(server->display, 1, wlr_renderer_get_drm_fd(server->renderer));
	}

	server->xdg_activation_v1 = wlr_xdg_activation_v1_create(server->display);
	server->listeners.xdg_activation_v1_request_activate.notify = handle_xdg_activation_v1_request_activate;
	wl_signal_add(&server->xdg_activation_v1->events.request_activate, &server->listeners.xdg_activation_v1_request_activate);
	server->listeners.xdg_activation_v1_new_token.notify = handle_xdg_activation_v1_new_token;
	wl_signal_add(&server->xdg_activation_v1->events.new_token, &server->listeners.xdg_activation_v1_new_token);

	server->xdg_shell = wlr_xdg_shell_create(server->display, 5);
	wl_signal_add(&server->xdg_shell->events.new_toplevel, &server->listeners.xdg_shell_toplevel);
	server->listeners.xdg_shell_toplevel.notify = handle_xdg_shell_toplevel;

	server->wlr_data_control_manager_v1 = wlr_data_control_manager_v1_create(server->display);
	server->drm_lease_v1_manager = wlr_drm_lease_v1_manager_create(server->display, server->backend);
	server->ext_data_control_manager_v1 = wlr_ext_data_control_manager_v1_create(server->display, 1);
	server->input_method_manager_v2 = wlr_input_method_manager_v2_create(server->display);
	server->text_input_manager_v3 = wlr_text_input_manager_v3_create(server->display);
	server->relative_pointer_manager_v1 = wlr_relative_pointer_manager_v1_create(server->display);
	server->security_context_manager_v1 = wlr_security_context_manager_v1_create(server->display);
	server->session_lock_manager_v1 = wlr_session_lock_manager_v1_create(server->display);
	server->text_input_manager_v3 = wlr_text_input_manager_v3_create(server->display);

	server->idle_inhibit_manager_v1 = wlr_idle_inhibit_v1_create(server->display);
	// wl_signal_add(&server->idle_inhibit_manager_v1->wlr_manager->events.new_inhibitor, &server->idle_inhibit_manager_v1->new_idle_inhibitor_v1);
	// server->idle_inhibit_manager_v1->new_idle_inhibitor_v1.notify = handle_new_idle_inhibitor_v1;
	// wl_signal_add(&server->idle_inhibit_manager_v1->wlr_manager->events.destroy, &server->idle_inhibit_manager_v1->manager_destroy);
	// server->idle_inhibit_manager_v1->manager_destroy.notify = handle_inle_inhibit_manager_destroy;
	// wl_list_init(&server->idle_inhibit_manager_v1->inhibitors);

	server->server_decoration_manager = wlr_server_decoration_manager_create(server->display);
	wlr_server_decoration_manager_set_default_mode(server->server_decoration_manager, WLR_SERVER_DECORATION_MANAGER_MODE_SERVER);
	wl_signal_add(&server->server_decoration_manager->events.new_decoration, &server->listeners.new_server_decoration);
	server->listeners.new_server_decoration.notify = handle_new_server_decoration;

	server->xdg_decoration_manager_v1 = wlr_xdg_decoration_manager_v1_create(server->display);
	wl_signal_add(&server->xdg_decoration_manager_v1->events.new_toplevel_decoration, &server->listeners.new_xdg_decoration);
	server->listeners.new_xdg_decoration.notify = handle_new_xdg_decoration;

	server->root = wlkit_root_create(server);

	wl_list_init(&server->outputs);
	wl_list_init(&server->inputs);
	wl_list_init(&server->workspaces);
	wl_list_init(&server->windows);
	wl_list_init(&server->decorations);
	wl_list_init(&server->xdg_decorations);

	// TODO focus manager
	// TODO keyboard manager
	// TODO portal manager

	server->listeners.new_output.notify = handle_new_output;
	wl_signal_add(&server->backend->events.new_output, &server->listeners.new_output);
	server->listeners.new_input.notify = handle_new_input;
	wl_signal_add(&server->backend->events.new_input, &server->listeners.new_input);
	server->listeners.new_xdg_surface.notify = handle_new_xdg_surface;
	wl_signal_add(&server->xdg_shell->events.new_surface, &server->listeners.new_xdg_surface);

	wl_list_init(&server->handlers.renderer_lost);
	wl_list_init(&server->handlers.new_output);
	wl_list_init(&server->handlers.new_input);
	wl_list_init(&server->handlers.new_xdg_surface);

	struct wlkit_server_handler * wrapper;
	wl_list_for_each(wrapper, &server->handlers.create, link) {
		wrapper->handler(server);
	}

	return server;
}

void wlkit_destroy(struct wlkit_server * server) {
	if (!server) {
		return;
	}

	struct wlkit_server_handler * wrapper;
	wl_list_for_each(wrapper, &server->handlers.destroy, link) {
		wrapper->handler(server);
	}

	struct wlkit_window * window, * tmp_window;
	wl_list_for_each_safe(window, tmp_window, &server->windows, link) {
		wlkit_window_destroy(window);
	}

	struct wlkit_workspace *workspace, *tmp_workspace;
	wl_list_for_each_safe(workspace, tmp_workspace, &server->workspaces, link) {
		wlkit_workspace_destroy(workspace, NULL);
	}

	if (server->keyboard_manager) {
		struct wlkit_keyboard_layout * layout, * tmp_layout;
		wl_list_for_each_safe(layout, tmp_layout, &server->keyboard_manager->layouts, link) {
			wlkit_keyboard_layout_destroy(layout);
		}
		free(server->keyboard_manager);
	}

	wlr_allocator_destroy(server->allocator);
	wlr_renderer_destroy(server->renderer);
	wlr_backend_destroy(server->backend);
	wl_event_loop_destroy(server->event_loop);

	free(server);
}

bool wlkit_start(struct wlkit_server * server) {
	if (!server) {
		return false;
	}

	server->socket = wl_display_add_socket_auto(server->display);
	if (!server->socket) {
		wlr_log(WLR_ERROR, "Failed to create Wayland socket");
		return false;
	}

	if (!wlr_backend_start(server->backend)) {
		wlr_log(WLR_ERROR, "Failed to start wlkit backend");
		return false;
	}

	setenv("WAYLAND_DISPLAY", server->socket, true);
	wlr_log(WLR_INFO, "Running wlkit on WAYLAND_DISPLAY=%s", server->socket);

	struct wlkit_server_handler * wrapper;
	wl_list_for_each(wrapper, &server->handlers.start, link) {
		wrapper->handler(server);
	}

	server->running = true;
	wl_display_run(server->display);

	return true;
}

void wlkit_stop(struct wlkit_server * server) {
	if (!server) {
		return;
	}

	struct wlkit_server_handler * wrapper;
	wl_list_for_each(wrapper, &server->handlers.stop, link) {
		wrapper->handler(server);
	}

	server->running = false;
	wl_display_terminate(server->display);
}

static void assume_handler(struct wl_list * handlers, wlkit_server_handler_t handler) {
	struct wlkit_server_handler * wrapper = malloc(sizeof(*wrapper));
	wrapper->handler = handler;
	wl_list_insert(handlers, &wrapper->link);
}

void wlkit_on_destroy(struct wlkit_server * server, wlkit_server_handler_t handler) {
	assume_handler(&server->handlers.destroy, handler);
}

void wlkit_on_start(struct wlkit_server * server, wlkit_server_handler_t handler) {
	assume_handler(&server->handlers.start, handler);
}

void wlkit_on_stop(struct wlkit_server * server, wlkit_server_handler_t handler) {
	assume_handler(&server->handlers.stop, handler);
}

void wlkit_on_new_output(struct wlkit_server * server, wlkit_notify_handler_t handler) {
	wlkit_assume_notify_handler(&server->handlers.new_output, handler);
}
