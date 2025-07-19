#pragma once

extern "C" {
#include <wlr/backend.h>
#include <wlr/backend/session.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/types/wlr_xdg_shell.h>
#define namespace namespace_
#include <wlr/types/wlr_layer_shell_v1.h>
#undef namespace
// #define class class_
// #include <wlr/xwayland.h>
// #undef class
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_virtual_keyboard_v1.h>
#include <wlr/types/wlr_virtual_pointer_v1.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_xdg_activation_v1.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_drm_lease_v1.h>
#include <wlr/types/wlr_ext_data_control_v1.h>
#define delete delete_
#include <wlr/types/wlr_input_method_v2.h>
#undef delete
#include <wlr/types/wlr_text_input_v3.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_security_context_v1.h>
#include <wlr/types/wlr_session_lock_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_ext_image_capture_source_v1.h>
}

#include "common.hpp"
#include "seat.hpp"
#include "workspace.hpp"

namespace wlkit {

class Server {
public:
	struct PendingXDGWindow {
		struct ::wl_listener listener;
		Server * server;
		Workspace * workspace;
	};

	using Handler = std::function<void(Server*)>;
	using OutputLayoutChangeHandler = std::function<
		void(Server * server, struct ::wlr_output_layout * layout)>;
	using NewOutputHandler = std::function<
		void(Output * output, struct ::wlr_output * wlr_output, Server * server)>;
	using NewInputHandler = std::function<
		void(Input * input, struct ::wlr_input_device * device, Server * server)>;
	using NewXDGShellSurfaceHandler = std::function<
		void(Window * window, struct ::wlr_xdg_surface * xdg_surface, Output * output)>;

private:
	Seat * _seat;

	struct ::wl_display * _display;
	struct ::wl_event_loop * _event_loop;
	struct ::wlr_backend * _backend;
	struct ::wlr_session * _session;
	struct ::wlr_renderer * _renderer;
	struct ::wlr_allocator * _allocator;
	struct ::wlr_compositor * _compositor;

	Root * _root;
	const char * _socket_id;
	bool _inside_wl;
	bool _running;
	Output * _preferred_output;
	std::list<Output*> _outputs;
	std::list<Input*> _inputs;
	std::list<Workspace*> _workspaces;
	std::list<Window*> _windows;
	WindowsHistory * _windows_history;
	// struct ::wl_list _decorations;
	// struct ::wl_list _xdg_decorations;
	void * _data;

	struct ::wlr_data_device_manager * _data_device_manager;
	struct ::wlr_output_layout * _output_layout;
	struct ::wlr_xdg_output_manager_v1 * _output_manager;
	struct ::wlr_xdg_shell * _xdg_shell;
	struct ::wlr_layer_shell_v1 * _layer_shell;
	// struct ::wlr_xwayland * _xwayland;

	struct ::wlr_idle_notifier_v1 * _idle_notifier;
	struct ::wlr_foreign_toplevel_manager_v1 * _foreign_toplevel_manager;
	struct ::wlr_gamma_control_manager_v1 * _gamma_control_manager;
	struct ::wlr_screencopy_manager_v1 * _screencopy_manager;
	struct ::wlr_virtual_keyboard_manager_v1 * _virtual_keyboard_manager;
	struct ::wlr_virtual_pointer_manager_v1 * _virtual_pointer_manager;
	struct ::wlr_server_decoration_manager * _decoration_manager;
	struct ::wlr_xdg_decoration_manager_v1 * _xdg_decoration_manager;
	struct ::wlr_linux_dmabuf_v1 * _linux_dmabuf;
	struct ::wlr_xdg_activation_v1 * _xdg_activation;
	struct ::wlr_data_control_manager_v1 * _wlr_data_control_manager;
	struct ::wlr_drm_lease_v1_manager * _drm_lease_manager;
	struct ::wlr_ext_data_control_manager_v1 * _ext_data_control_manager;
	struct ::wlr_input_method_manager_v2 * _input_method_manager;
	struct ::wlr_text_input_manager_v3 * _text_input_manager;
	struct ::wlr_relative_pointer_manager_v1 * _relative_pointer_manager;
	struct ::wlr_security_context_manager_v1 * _security_context_manager;
	struct ::wlr_session_lock_manager_v1 * _session_lock_manager;
	struct ::wlr_idle_inhibit_manager_v1 * _idle_inhibit_manager;
	struct ::wlr_ext_output_image_capture_source_manager_v1 * _ext_output_image_capture_source_manager;
	// struct ::wlr_content_type_manager_v1 * content_type_manager;
	// struct wlr_ext_image_copy_capture_manager_v1 * ext_image_copy_capture_manager_v1;
	// struct wlr_output_power_manager_v1 * output_power_manager_v1;
	// struct wlr_tablet_manager_v2 * tablet_manager_v2;
	// struct wlr_tearing_control_manager_v1 * tearing_control_manager_v1;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	std::list<Handler> _on_start;
	std::list<Handler> _on_stop;
	std::list<OutputLayoutChangeHandler> _on_output_layout_change;
	std::list<NewOutputHandler> _on_new_output;
	std::list<NewInputHandler> _on_new_input;
	std::list<NewXDGShellSurfaceHandler> _on_new_xdg_shell_surface;
	std::list<NewXDGShellSurfaceHandler> _on_new_xdg_shell_toplevel;
	std::list<NewXDGShellSurfaceHandler> _on_new_xdg_shell_popup;

	struct ::wl_listener _destroy_listener;
	struct ::wl_listener _output_layout_change_listener;
	struct ::wl_listener _new_output_listener;
	struct ::wl_listener _new_input_listener;
	struct ::wl_listener _new_xdg_shell_surface_listener;
	struct ::wl_listener _new_xdg_shell_toplevel_listener;
	struct ::wl_listener _new_xdg_shell_popup_listener;
	struct ::wl_listener _new_layer_shell_surface_listener;
	struct ::wl_listener _new_xwayland_surface_listener;
	struct ::wl_listener _new_virtual_keyboard_listener;
	struct ::wl_listener _new_virtual_pointer_listener;
	struct ::wl_listener _new_decoration_listener;
	struct ::wl_listener _new_xdg_toplevel_decoration_listener;

	struct ::wl_listener _renderer_lost_listener;
	struct ::wl_listener _xdg_activation_destroy_listener;
	struct ::wl_listener _xdg_activation_request_activate_listener;
	struct ::wl_listener _xdg_activation_new_token_listener;
	// struct ::wl_listener _new_server_decoration_listener;
	// struct ::wl_listener _new_xdg_decoration_listener;
	// struct ::wl_listener _new_output_capture_request_listener;

public:
	Server(
		Seat * seat,
		const Handler & callback);

	~Server();

	Workspace * get_workspace_by_id(Workspace::ID id);

	Server & start();
	Server & stop();
	Server & add_workspace(Workspace * workspace);
	Server & add_window(Window * window);
	Server & remove_workspace(Workspace * workspace);
	Server & remove_window(Window * window);
	Server & prefer_output(Output * output);

	[[nodiscard]] struct ::wl_display * display() const;
	[[nodiscard]] struct ::wl_event_loop * event_loop() const;
	[[nodiscard]] struct ::wlr_backend * backend() const;
	[[nodiscard]] struct ::wlr_session * session() const;
	[[nodiscard]] struct ::wlr_renderer * renderer() const;
	[[nodiscard]] struct ::wlr_allocator * allocator() const;
	[[nodiscard]] struct ::wlr_compositor * compositor() const;

	[[nodiscard]] Seat * seat() const;
	[[nodiscard]] Root * root() const;
	[[nodiscard]] const char * socket_id() const;
	[[nodiscard]] bool inside_wl() const;
	[[nodiscard]] bool running() const;
	[[nodiscard]] Output * preferred_output() const;
	[[nodiscard]] void * data() const;

	[[nodiscard]] std::list<Output*> outputs() const;
	[[nodiscard]] std::list<Input*> inputs() const;
	[[nodiscard]] std::list<Workspace*> workspaces() const;
	[[nodiscard]] std::list<Window*> windows() const;
	[[nodiscard]] WindowsHistory * windows_history() const;

	[[nodiscard]] struct ::wlr_xdg_shell * xdg_shell() const;

	Server & set_data(void * data);
	// TODO setters

	Server & on_destroy(const Handler & handler);
	Server & on_start(const Handler & handler);
	Server & on_stop(const Handler & handler);
	Server & on_output_layout_change(const OutputLayoutChangeHandler & handler);
	Server & on_new_output(const NewOutputHandler & handler);
	Server & on_new_input(const NewInputHandler & handler);
	Server & on_new_xdg_shell_surface(const NewXDGShellSurfaceHandler & handler);
	Server & on_new_xdg_shell_toplevel(const NewXDGShellSurfaceHandler & handler);
	Server & on_new_xdg_shell_popup(const NewXDGShellSurfaceHandler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
	static void _handle_output_layout_change(struct ::wl_listener * listener, void * data);
	static void _handle_new_output(struct ::wl_listener * listener, void * data);
	static void _handle_new_input(struct ::wl_listener * listener, void * data);
	static void _handle_new_xdg_shell_surface(struct ::wl_listener * listener, void * data);
	static void _handle_new_xdg_shell_toplevel(struct ::wl_listener * listener, void * data);
	static void _handle_new_xdg_shell_popup(struct ::wl_listener * listener, void * data);
	static void _handle_new_layer_shell_surface(struct ::wl_listener * listener, void * data);
	static void _handle_new_xwayland_surface(struct ::wl_listener * listener, void * data);
	static void _handle_new_virtual_keyboard(struct ::wl_listener * listener, void * data);
	static void _handle_new_virtual_pointer(struct ::wl_listener * listener, void * data);
	static void _handle_new_decoration(struct ::wl_listener * listener, void * data);
	static void _handle_new_xdg_toplevel_decoration(struct ::wl_listener * listener, void * data);

	static void _handle_xdg_activation_destroy(struct ::wl_listener * listener, void * data);
	static void _handle_xdg_activation_request_activate(struct ::wl_listener * listener, void * data);
	static void _handle_xdg_activation_new_token(struct ::wl_listener * listener, void * data);
	static void _handle_new_server_decoration(struct ::wl_listener * listener, void * data);
	static void _handle_new_xdg_decoration(struct ::wl_listener * listener, void * data);
};

}
