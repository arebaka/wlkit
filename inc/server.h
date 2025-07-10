#ifndef WLKIT_SERVER_H
#define WLKIT_SERVER_H

#include "common.h"

#include <wlr/backend.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/backend/session.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>

#include <wlr/types/wlr_compositor.h>
// #include <wlr/types/wlr_content_type_v1.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm_lease_v1.h>
#include <wlr/types/wlr_ext_data_control_v1.h>
#include <wlr/types/wlr_ext_image_capture_source_v1.h>
// #include <wlr/types/wlr_ext_image_copy_capture_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_input_method_v2.h>
// #include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
// #include <wlr/types/wlr_output_power_management_v1.h>
// #include <wlr/types/wlr_pointer_constraints_v1.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_security_context_v1.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_session_lock_v1.h>
// #include <wlr/types/wlr_tablet_v2.h>
// #include <wlr/types/wlr_tearing_control_v1.h>
#include <wlr/types/wlr_text_input_v3.h>
#include <wlr/types/wlr_xdg_activation_v1.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_xdg_shell.h>

struct wlkit_server;

struct wlkit_server {
	struct wl_display * wl_display;
	struct wl_event_loop * wl_event_loop;
	struct wlr_seat * seat;

	struct wlr_backend * backend;
	struct wlr_session * session;
	struct wlr_backend * headless_backend;
	struct wlr_renderer * renderer;
	struct wlr_allocator * allocator;

	struct wlr_compositor * compositor;
	struct wlr_data_device_manager * data_device_manager;
	struct wlr_idle_notifier_v1 * idle_notifier_v1;
	struct wlr_linux_dmabuf_v1 * linux_dmabuf_v1;
	struct wlr_xdg_activation_v1 *xdg_activation_v1;
	// struct wlr_pointer_constraints_v1 * pointer_constraints_v1;

	// struct wlr_layer_shell_v1 * layer_shell_v1;
	struct wlr_xdg_shell * xdg_shell;

	// struct wlr_content_type_manager_v1 * content_type_manager_v1;
	struct wlr_data_control_manager_v1 * wlr_data_control_manager_v1;
	struct wlr_drm_lease_v1_manager * drm_lease_v1_manager;
	struct wlr_ext_data_control_manager_v1 * ext_data_control_manager_v1;
	struct wlr_ext_foreign_toplevel_image_capture_source_manager_v1 * ext_foreign_toplevel_image_capture_source_manager_v1;
	// struct wlr_ext_image_copy_capture_manager_v1 * ext_image_copy_capture_manager_v1;
	struct wlr_idle_inhibit_manager_v1 * idle_inhibit_manager_v1;
	struct wlr_input_method_manager_v2 * input_method_manager_v2;
	// struct wlr_output_power_manager_v1 * output_power_manager_v1;
	struct wlr_relative_pointer_manager_v1 * relative_pointer_manager_v1;
	struct wlr_security_context_manager_v1 * security_context_manager_v1;
	struct wlr_server_decoration_manager * server_decoration_manager;
	struct wlr_session_lock_manager_v1 * session_lock_manager_v1;
	// struct wlr_tablet_manager_v2 * tablet_manager_v2;
	// struct wlr_tearing_control_manager_v1 * tearing_control_manager_v1;
	struct wlr_text_input_manager_v3 * text_input_manager_v3;
	struct wlr_xdg_decoration_manager_v1 * xdg_decoration_manager_v1;

	struct wlkit_root * root;
	struct wlkit_workspace * current_workspace;

	struct wl_list outputs;
	struct wl_list inputs;
	struct wl_list workspaces;
	struct wl_list windows;

	struct wlkit_focus_manager * focus_manager;
	struct wlkit_keyboard_manager * keyboard_manager;
	struct wlkit_portal_manager * portal_manager;

	struct {
		struct wl_listener renderer_lost;
		struct wl_listener new_output;
		struct wl_listener new_input;
		struct wl_listener new_xdg_surface;
	} listeners;

	struct {
		wlkit_handler_t create;
		wlkit_handler_t destroy;
		wlkit_notify_handler_t renderer_lost;
		wlkit_notify_handler_t new_output;
		wlkit_notify_handler_t new_input;
		wlkit_notify_handler_t new_xdg_surface;
		wlkit_notify_handler_t output_frame;
	} handlers;

	const char * socket;
	bool running;

	void * user_data;
};

struct wlkit_server * wlkit_create(
	struct wl_display * display,
	struct wlr_seat * seat,
	wlkit_handler_t callback
);

void wlkit_destroy(
	struct wlkit_server * server
);

bool wlkit_start(
	struct wlkit_server * server
);

void wlkit_run(
	struct wlkit_server * server
);

void wlkit_stop(
	struct wlkit_server * server
);

void wlkit_on_destroy(
	struct wlkit_server * server,
	wlkit_handler_t handler
);

void wlkit_on_new_ouput(
	struct wlkit_server * server,
	wlkit_notify_handler_t handler
);

void wlkit_on_ouput_frame(
	struct wlkit_server * server,
	wlkit_notify_handler_t handler
);

#endif // WLKIT_SERVER_H
