#include "server.hpp"
#include "output.hpp"
#include "root.hpp"

extern "C" {
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_linux_drm_syncobj_v1.h>
}

using namespace wlkit;

Server::Server(struct wl_display & display, struct wlr_seat & seat, const Handler & callback):
_display(&display), _seat(&seat), _running(false), _data(nullptr) {
	if (!_display || !_seat) {
		// TODO error
	}

	_event_loop = wl_display_get_event_loop(_display);

	_backend = wlr_backend_autocreate(_event_loop, &_session);
	if (!_backend) {
		// TODO error
	}

	_renderer = wlr_renderer_autocreate(_backend);
	if (!_renderer) {
		// TODO error
	}
	wlr_renderer_init_wl_display(_renderer, _display);

	_allocator = wlr_allocator_autocreate(_backend, _renderer);
	if (!_allocator) {
		// TODO error
	}

	_root = new Root(*this, nullptr, 24);  // TODO from config

	_compositor = wlr_compositor_create(_display, 6, _renderer);
	wlr_subcompositor_create(_display);

	_xdg_shell = wlr_xdg_shell_create(_display, 5);
	_new_xdg_shell_toplevel_listener.notify = _handle_new_xdg_shell_toplevel;
	wl_signal_add(&_xdg_shell->events.new_toplevel, &_new_xdg_shell_toplevel_listener);

	// _data_device_manager = wlr_data_device_manager_create(_display);
	// _idle_notifier_v1 = wlr_idle_notifier_v1_create(_display);

	// if (wlr_renderer_get_texture_formats(_renderer, WLR_BUFFER_CAP_DMABUF) != nullptr) {
	// 	_linux_dmabuf_v1 = wlr_linux_dmabuf_v1_create_with_renderer(_display, 4, _renderer);
	// }
	// if (wlr_renderer_get_drm_fd(_renderer) >= 0 &&
	// 	_renderer->features.timeline &&
	// 	_backend->features.timeline
	// ) {
	// 	wlr_linux_drm_syncobj_manager_v1_create(_display, 1, wlr_renderer_get_drm_fd(_renderer));
	// }

	// _xdg_activation_v1 = wlr_xdg_activation_v1_create(_display);
	// _xdg_activation_v1_destroy_listener.notify = _handle_xdg_activation_v1_destroy;
	// wl_signal_add(&_xdg_activation_v1->events.destroy, &_xdg_activation_v1_destroy_listener);
	// _xdg_activation_v1_request_activate_listener.notify = _handle_xdg_activation_v1_request_activate;
	// wl_signal_add(&_xdg_activation_v1->events.request_activate, &_xdg_activation_v1_request_activate_listener);
	// _xdg_activation_v1_new_token_listener.notify = _handle_xdg_activation_v1_new_token;
	// wl_signal_add(&_xdg_activation_v1->events.new_token, &_xdg_activation_v1_new_token_listener);

	// _wlr_data_control_manager_v1 = wlr_data_control_manager_v1_create(_display);
	// _drm_lease_v1_manager = wlr_drm_lease_v1_manager_create(_display, _backend);
	// _ext_data_control_manager_v1 = wlr_ext_data_control_manager_v1_create(_display, 1);
	// _input_method_manager_v2 = wlr_input_method_manager_v2_create(_display);
	// _text_input_manager_v3 = wlr_text_input_manager_v3_create(_display);
	// _relative_pointer_manager_v1 = wlr_relative_pointer_manager_v1_create(_display);
	// _security_context_manager_v1 = wlr_security_context_manager_v1_create(_display);
	// _session_lock_manager_v1 = wlr_session_lock_manager_v1_create(_display);
	// _text_input_manager_v3 = wlr_text_input_manager_v3_create(_display);

	// _idle_inhibit_manager_v1 = wlr_idle_inhibit_v1_create(_display);
	// // wl_signal_add(&_idle_inhibit_manager_v1->wlr_manager->events.new_inhibitor, &_idle_inhibit_manager_v1->new_idle_inhibitor_v1);
	// // _idle_inhibit_manager_v1->new_idle_inhibitor_v1.notify = handle_new_idle_inhibitor_v1;
	// // wl_signal_add(&_idle_inhibit_manager_v1->wlr_manager->events.destroy, &_idle_inhibit_manager_v1->manager_destroy);
	// // _idle_inhibit_manager_v1->manager_destroy.notify = handle_inle_inhibit_manager_destroy;
	// // wl_list_init(&_idle_inhibit_manager_v1->inhibitors);

	// _server_decoration_manager = wlr_server_decoration_manager_create(_display);
	// wlr_server_decoration_manager_set_default_mode(_server_decoration_manager, WLR_SERVER_DECORATION_MANAGER_MODE_SERVER);
	// _new_server_decoration_listener.notify = _handle_new_server_decoration;
	// wl_signal_add(&_server_decoration_manager->events.new_decoration, &_new_server_decoration_listener);

	// _xdg_decoration_manager_v1 = wlr_xdg_decoration_manager_v1_create(_display);
	// _new_xdg_decoration_listener.notify = _handle_new_xdg_decoration;
	// wl_signal_add(&_xdg_decoration_manager_v1->events.new_toplevel_decoration, &_new_xdg_decoration_listener);

    _new_output_listener.notify = _handle_new_output;
    wl_signal_add(&_backend->events.new_output, &_new_output_listener);
	_new_input_listener.notify = _handle_new_input;
	wl_signal_add(&_backend->events.new_input, &_new_input_listener);
	_new_xdg_surface_listener.notify = _handle_new_xdg_surface;
	wl_signal_add(&_xdg_shell->events.new_surface, &_new_xdg_surface_listener);

	if (callback) {
		_on_create.push_back(std::move(callback));
	}
	for (auto & cb : _on_create) {
		cb(*this);
	}
}

Server::~Server() {
	for (auto & cb : _on_destroy) {
		cb(*this);
	}

	wlr_allocator_destroy(_allocator);
	wlr_renderer_destroy(_renderer);
	wlr_backend_destroy(_backend);
	wl_event_loop_destroy(_event_loop);
}

Server & Server::start() {
	_socket_id = wl_display_add_socket_auto(_display);
	if (!_socket_id) {
		// TODO error
	}

	if (!wlr_backend_start(_backend)) {
		// TODO error
	}

	setenv("WAYLAND_DISPLAY", _socket_id, true);
	wlr_log(WLR_INFO, "Running wlkit on WAYLAND_DISPLAY=%s", _socket_id);

	for (auto & cb : _on_start) {
		cb(*this);
	}

	_running = true;
	wl_display_run(_display);

	return *this;
}

Server & Server::stop() {
	for (auto & cb : _on_stop) {
		cb(*this);
	}

	_running = false;
	wl_display_terminate(_display);

	return *this;
}

wl_display * Server::display() const {
	return _display;
}

wl_event_loop * Server::event_loop() const {
	return _event_loop;
}

wlr_seat * Server::seat() const {
	return _seat;
}

wlr_backend * Server::backend() const {
	return _backend;
}

wlr_session * Server::session() const {
	return _session;
}

wlr_renderer * Server::renderer() const {
	return _renderer;
}

wlr_allocator * Server::allocator() const {
	return _allocator;
}

wlr_compositor * Server::compositor() const {
	return _compositor;
}

wlr_xdg_shell * Server::xdg_shell() const {
	return _xdg_shell;
}

Root * Server::root() const {
	return _root;
}

const char * Server::socket_id() const {
	return _socket_id;
}

bool Server::running() const {
	return _running;
}

void * Server::data() const {
	return _data;
}

Server & Server::set_data(void * data) {
	_data = data;
	return *this;
}

Server & Server::add_workspace(Workspace * workspace) {
	_workspaces.push_back(workspace);
}

Server & Server::on_destroy(const Handler & handler) {
	_on_destroy.push_back(std::move(handler));
	return *this;
}

Server & Server::on_start(const Handler & handler) {
	_on_start.push_back(std::move(handler));
	return *this;
}

Server & Server::on_stop(const Handler & handler) {
	_on_stop.push_back(std::move(handler));
	return *this;
}

Server & Server::on_new_output(const NotifyHandler & handler) {
	_on_new_output.push_back(std::move(handler));
	return *this;
}

void Server::_handle_new_xdg_shell_toplevel(struct wl_listener * listener, void * data) {}

void Server::_handle_new_output(struct wl_listener * listener, void * data) {
	Server * server = wl_container_of(listener, server, _new_output_listener);
	auto wlr_output = static_cast<struct wlr_output*>(data);

	auto output = new Output(*server, *wlr_output);
	server->_outputs.push_back(output);

	Object object = { .output = output };
	for (auto & cb : server->_on_new_output) {
		cb(listener, data, object);
	}
}

void Server::_handle_new_input(struct wl_listener * listener, void * data) {

}

void Server::_handle_new_xdg_surface(struct wl_listener * listener, void * data) {
	Server * server = wl_container_of(listener, server, _new_xdg_surface_listener);
	auto xdg_surface = static_cast<struct wlr_xdg_surface *>(data);

	if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		return;
	}

	// struct wlkit_window * window = malloc(sizeof(*window));
	// if (!window) {
	// 	wlr_log(WLR_ERROR, "Unable to allocate wlkit window");
	// 	return;
	// }

	// window->server = server;
	// window->xdg_surface = xdg_surface;
	// window->surface = xdg_surface->surface;
	// window->workspace = server->current_workspace;

	// wl_list_insert(&server->current_workspace->windows, &window->link);
	// wl_list_insert(&server->windows, &window->link);

	// window->listeners.map.notify = NULL;
	// window->listeners.unmap.notify = NULL;
	// window->listeners.destroy.notify = NULL;

	// if (server->portal_manager) {
	// 	window->foreign_toplevel = wlr_foreign_toplevel_handle_v1_create(
	// 		server->portal_manager->foreign_toplevel_manager_v1);
	// }
}

void Server::_handle_xdg_activation_v1_destroy(struct wl_listener * listener, void * data) {};
void Server::_handle_xdg_activation_v1_request_activate(struct wl_listener * listener, void * data) {};
void Server::_handle_xdg_activation_v1_new_token(struct wl_listener * listener, void * data) {};
void Server::_handle_new_server_decoration(struct wl_listener * listener, void * data) {};
void Server::_handle_new_xdg_decoration(struct wl_listener * listener, void * data) {};
