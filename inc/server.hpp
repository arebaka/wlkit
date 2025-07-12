#pragma once

#include <list>

extern "C" {
#include <wlr/backend.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/backend/session.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_xdg_activation_v1.h>

#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_drm_lease_v1.h>
#include <wlr/types/wlr_ext_data_control_v1.h>
#include <wlr/types/wlr_ext_image_capture_source_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#define delete delete_
#include <wlr/types/wlr_input_method_v2.h>
#undef delete
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_security_context_v1.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_session_lock_v1.h>
#include <wlr/types/wlr_text_input_v3.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
}

#include "common.hpp"

namespace wlkit {

class Server {
public:
	typedef std::function<void(Server&)> Handler;

private:
	struct wl_display * _display;
	struct wl_event_loop * _event_loop;

	struct wlr_seat * _seat;
	struct wlr_backend * _backend;
	struct wlr_session * _session;
	struct wlr_renderer * _renderer;
	struct wlr_allocator * _allocator;
	struct wlr_compositor * _compositor;

	struct wlr_xdg_shell * _xdg_shell;
	// struct wlr_data_device_manager * _data_device_manager;
	// struct wlr_idle_notifier_v1 * _idle_notifier_v1;
	// struct wlr_linux_dmabuf_v1 * _linux_dmabuf_v1;
	// struct wlr_xdg_activation_v1 * _xdg_activation_v1;
	// // struct wlr_pointer_constraints_v1 * pointer_constraints_v1;

	// // struct wlr_content_type_manager_v1 * content_type_manager_v1;
	// struct wlr_data_control_manager_v1 * _wlr_data_control_manager_v1;
	// struct wlr_drm_lease_v1_manager * _drm_lease_v1_manager;
	// struct wlr_ext_data_control_manager_v1 * _ext_data_control_manager_v1;
	// struct wlr_ext_foreign_toplevel_image_capture_source_manager_v1 * _ext_foreign_toplevel_image_capture_source_manager_v1;
	// // struct wlr_ext_image_copy_capture_manager_v1 * ext_image_copy_capture_manager_v1;
	// struct wlr_idle_inhibit_manager_v1 * _idle_inhibit_manager_v1;
	// struct wlr_input_method_manager_v2 * _input_method_manager_v2;
	// // struct wlr_output_power_manager_v1 * output_power_manager_v1;
	// struct wlr_relative_pointer_manager_v1 * _relative_pointer_manager_v1;
	// struct wlr_security_context_manager_v1 * _security_context_manager_v1;
	// struct wlr_server_decoration_manager * _server_decoration_manager;
	// struct wlr_session_lock_manager_v1 * _session_lock_manager_v1;
	// // struct wlr_tablet_manager_v2 * tablet_manager_v2;
	// // struct wlr_tearing_control_manager_v1 * tearing_control_manager_v1;
	// struct wlr_text_input_manager_v3 * _text_input_manager_v3;
	// struct wlr_xdg_decoration_manager_v1 * _xdg_decoration_manager_v1;

	Root * _root;
	bool _running;
	const char * _socket_id;
	void * _data;

	std::list<Output*> _outputs;
	// struct wl_list _inputs;
	std::list<Workspace*> _workspaces;
	struct wl_list _windows;
	// WindowsHistory _windows_history();
	// struct wl_list _decorations;
	// struct wl_list _xdg_decorations;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	std::list<Handler> _on_start;
	std::list<Handler> _on_stop;
	std::list<NotifyHandler> _on_new_output;

	struct wl_listener _renderer_lost_listener;
	struct wl_listener _new_xdg_shell_toplevel_listener;
	struct wl_listener _new_output_listener;
	struct wl_listener _new_input_listener;
	struct wl_listener _new_xdg_surface_listener;
	// struct wl_listener _xdg_activation_v1_destroy_listener;
	// struct wl_listener _xdg_activation_v1_request_activate_listener;
	// struct wl_listener _xdg_activation_v1_new_token_listener;
	// struct wl_listener _xdg_shell_toplevel_listener;
	// struct wl_listener _new_foreign_toplevel_capture_request_listener;
	// struct wl_listener _new_server_decoration_listener;
	// struct wl_listener _new_xdg_decoration_listener;

public:
	Server(
		struct wl_display & display,
		struct wlr_seat & seat,
		const Handler & callback);

	~Server();

	Server & start();
	Server & stop();
	// Workspace * get_workspace_by_id(Workspace::ID id);

	Server & add_workspace(Workspace * workspace);

	wl_display * display() const;
	wl_event_loop * event_loop() const;
	wlr_seat * seat() const;
	wlr_backend * backend() const;
	wlr_session * session() const;
	wlr_renderer * renderer() const;
	wlr_allocator * allocator() const;
	wlr_compositor * compositor() const;
	wlr_xdg_shell * xdg_shell() const;
	Root * root() const;
	const char * socket_id() const;
	bool running() const;
	void * data() const;

	Server & set_data(void * data);

	Server & on_destroy(const Handler & handler);
	Server & on_start(const Handler & handler);
	Server & on_stop(const Handler & handler);
	Server & on_new_output(const NotifyHandler & handler);

private:
	static void _handle_new_xdg_shell_toplevel(struct wl_listener * listener, void * data);
	static void _handle_new_output(struct wl_listener * listener, void * data);
	static void _handle_new_input(struct wl_listener * listener, void * data);
	static void _handle_new_xdg_surface(struct wl_listener * listener, void * data);
	static void _handle_xdg_activation_v1_destroy(struct wl_listener * listener, void * data);
	static void _handle_xdg_activation_v1_request_activate(struct wl_listener * listener, void * data);
	static void _handle_xdg_activation_v1_new_token(struct wl_listener * listener, void * data);
	static void _handle_new_server_decoration(struct wl_listener * listener, void * data);
	static void _handle_new_xdg_decoration(struct wl_listener * listener, void * data);

};

}
