#include "wlkit.h"

#include <wayland-server-core.h>
#include <wayland-util.h>
#include <wlr/types/wlr_linux_drm_syncobj_v1.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_subcompositor.h>

static void handle_lauout_destroy(struct wl_listener * listener, void * data) {
}

static void handle_output_frame(struct wl_listener * listener, void * data) {
	struct wlkit_output * output = wl_container_of(listener, output, listeners.frame);
	struct wlkit_server * server = output->server;

	if (server->handlers.output_frame) {
		server->handlers.output_frame(listener, data, server);
	}
}

static void handle_output_destroy(struct wl_listener * listener, void * data) {
}

static void handle_output_present(struct wl_listener * listener, void * data) {
}

static void handle_ouput_request_state(struct wl_listener * listener, void * data) {
}

static int handle_output_repaint_timer(void * data) {
}

static void handle_new_output(struct wl_listener * listener, void * data) {
    struct wlkit_server * server = wl_container_of(listener, server, listeners.new_output);
    struct wlr_output * wlr_output = data;

	struct wlr_scene_output * scene_output = wlr_scene_output_create(server->root->scene, wlr_output);
	if (!scene_output) {
		wlr_log(WLR_ERROR, "Failed to create a wlkit scene output");
		return;
	}

    struct wlkit_output * output = wlkit_output_create(server, wlr_output, scene_output);
	if (!output) {
		wlr_log(WLR_ERROR, "Failed to create a wlkit output");
		wlr_scene_output_destroy(scene_output);
		return;
	}

    if (!wlr_output_init_render(wlr_output, server->allocator, server->renderer)) {
		wlr_log(WLR_ERROR, "Failed to init wlkit output render");
		return;
	}

	wl_signal_add(&server->root->output_layout->events.destroy, &output->listeners.layout_destroy);
	output->listeners.layout_destroy.notify = handle_lauout_destroy;
	wl_signal_add(&wlr_output->events.frame, &output->listeners.frame);
	output->listeners.frame.notify = handle_output_frame;
	wl_signal_add(&wlr_output->events.destroy, &output->listeners.destroy);
	output->listeners.destroy.notify = handle_output_destroy;
	wl_signal_add(&wlr_output->events.present, &output->listeners.present);
	output->listeners.present.notify = handle_output_present;
	wl_signal_add(&wlr_output->events.request_state, &output->listeners.request_state);
	output->listeners.request_state.notify = handle_ouput_request_state;

	output->repaint_timer = wl_event_loop_add_timer(server->wl_event_loop, handle_output_repaint_timer, output);

    wlr_output_layout_add_auto(server->root->output_layout, wlr_output);
    wl_list_insert(&server->outputs, &output->link);

	if (server->handlers.new_output) {
		server->handlers.new_output(listener, data, server);
	}
}
/*
static void handle_new_input(struct wl_listener * listener, void * data) {
    struct wlkit_server * server = wl_container_of(listener, server, new_input);
    struct wlr_input_device * device = data;

    struct wlkit_input * input = malloc(sizeof(*input));
    input->server = server;
    input->device = device;

    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        wlr_seat_set_keyboard(server->seat, device);
        break;
    case WLR_INPUT_DEVICE_POINTER:
        wlr_seat_set_pointer(server->seat, device);
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

    struct wlkit_window *window = malloc(sizeof(*window));
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

    if (window_callbacks && window_callbacks->on_new_window) {
        window_callbacks->on_new_window(window);
    }
}
*/
struct wlkit_server * wlkit_create(struct wl_display * display, struct wlr_seat * seat, wlkit_handler_t callback) {
    struct wlkit_server * server = malloc(sizeof(*server));
    if (!server || !display || !seat) {
        return NULL;
    }

    server->wl_display = display;
    server->seat = seat;

	server->wl_event_loop = wl_display_get_event_loop(server->wl_display);

	server->backend = wlr_backend_autocreate(server->wl_event_loop, &server->session);
	if (!server->backend) {
		wlr_log(WLR_ERROR, "Unable to create wlkit backend");
		return false;
	}

	server->renderer = wlr_renderer_autocreate(server->backend);
	if (!server->renderer) {
		wlr_log(WLR_ERROR, "Unable to create wlkit renderer");
		return false;
	}

	server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);
	if (!server->allocator) {
		wlr_log(WLR_ERROR, "Unable to create wlkit allocator");
		return false;
	}

	server->compositor = wlr_compositor_create(server->wl_display, 6, server->renderer);
	wlr_subcompositor_create(server->wl_display);

	server->data_device_manager = wlr_data_device_manager_create(server->wl_display);

	server->idle_notifier_v1 = wlr_idle_notifier_v1_create(server->wl_display);

	if (wlr_renderer_get_texture_formats(server->renderer, WLR_BUFFER_CAP_DMABUF) != NULL) {
		server->linux_dmabuf_v1 = wlr_linux_dmabuf_v1_create_with_renderer(server->wl_display, 4, server->renderer);
	}
	if (wlr_renderer_get_drm_fd(server->renderer) >= 0 &&
		server->renderer->features.timeline &&
		server->backend->features.timeline
	) {
		wlr_linux_drm_syncobj_manager_v1_create(server->wl_display, 1, wlr_renderer_get_drm_fd(server->renderer));
	}

	server->xdg_activation_v1 = wlr_xdg_activation_v1_create(server->wl_display);
	// server->xdg_activation_v1_request_activate.notify = xdg_activation_v1_handle_request_activate;
	// wl_signal_add(&server->xdg_activation_v1->events.request_activate, &server->xdg_activation_v1_request_activate);
	// server->xdg_activation_v1_new_token.notify = xdg_activation_v1_handle_new_token;
	// wl_signal_add(&server->xdg_activation_v1->events.new_token, &server->xdg_activation_v1_new_token);

	server->xdg_shell = wlr_xdg_shell_create(server->wl_display, 5);
	// wl_signal_add(&server->xdg_shell->events.new_toplevel, &server->xdg_shell_toplevel);
	// server->xdg_shell_toplevel.notify = handle_xdg_shell_toplevel;

	server->root = wlkit_root_create(server);

    server->current_workspace = wlkit_workspace_create(server, &wlkit_layout_floating, 1, "default");  // TODO
	// server->current_workspace = NULL;

	wl_list_init(&server->outputs);
    wl_list_init(&server->inputs);
    wl_list_init(&server->workspaces);
    wl_list_init(&server->windows);

	// TODO focus manager
	// TODO keyboard manager
	// TODO portal manager

	server->handlers.create = callback;

    server->listeners.new_output.notify = handle_new_output;
    wl_signal_add(&server->backend->events.new_output, &server->listeners.new_output);
	server->handlers.new_output = NULL;

    // server->new_input.notify = handle_new_input;
    // wl_signal_add(&backend->events.new_input, &server->new_input);
	// server->handlers.new_input = NULL;

    // server->new_xdg_surface.notify = handle_new_xdg_surface;
    // wl_signal_add(&server->xdg_shell->events.new_surface, &server->new_xdg_surface);
	// server->handlers.new_xdg_surface = NULL;

    server->running = false;
	server->user_data = NULL;

	if (server->handlers.create) {
		server->handlers.create(server);
	}

    return server;
}

void wlkit_destroy(struct wlkit_server * server) {
    if (!server) {
        return;
    }

	if (server->handlers.destroy) {
		server->handlers.destroy(server);
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
    wl_event_loop_destroy(server->wl_event_loop);

    free(server);
}

bool wlkit_start(struct wlkit_server * server) {
    if (!server) {
        return false;
    }

    server->socket = wl_display_add_socket_auto(server->wl_display);
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

    server->running = true;
    return true;
}

void wlkit_run(struct wlkit_server * server) {
    if (!server || !server->running) {
        return;
    }

    wl_display_run(server->wl_display);
}

void wlkit_stop(struct wlkit_server * server) {
    if (!server) {
        return;
    }

    server->running = false;
    wl_display_terminate(server->wl_display);
}

void wlkit_on_destroy(struct wlkit_server * server, wlkit_handler_t handler) {
	server->handlers.destroy = handler;
}

void wlkit_on_new_ouput(struct wlkit_server * server, wlkit_notify_handler_t handler) {
	server->handlers.new_output = handler;
}

void wlkit_on_ouput_frame(struct wlkit_server * server, wlkit_notify_handler_t handler) {
	server->handlers.output_frame = handler;
};
