#include "window.hpp"
#include "workspace.hpp"
#include "server.hpp"

#include <wlroots-0.19/wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>

using namespace wlkit;

Window::Window(Server & server, Workspace & workspace,
	struct wlr_xdg_surface & xdg_surface, struct wlr_xwayland_surface & xwayland_surface,
	const char * title, const char * app_id, const Handler & callback):
_server(&server), _workspace(&workspace),
_xdg_surface(&xdg_surface), _xwayland_surface(&xwayland_surface),
_data(nullptr) {
	_title = strdup(title ? title : "");
	_app_id = strdup(app_id ? app_id : "");
	_workspaces_history = new WorkspacesHistory();
	_mapped = false;
	_surface = _xdg_surface->surface;

	// _foreign_toplevel = wlr_foreign_toplevel_handle_v1_create(_server->foreign_toplevel_manager_v1());
	// wlr_foreign_toplevel_handle_v1_set_title(_foreign_toplevel, _title);
	// wlr_foreign_toplevel_handle_v1_set_app_id(_foreign_toplevel, _app_id);

	// _request_close_listener.notify = _handle_request_close;
	// wl_signal_add(&_foreign_toplevel->events.request_close, &_request_close_listener);
	// TODO handlers

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_xdg_surface->events.destroy, &_destroy_listener);
	wl_signal_add(&_surface->events.destroy, &_destroy_listener);
	// wl_signal_add(&_foreign_toplevel->events.destroy, &_destroy_listener);

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(*this);
	}
}

Window::~Window() {
	for (auto & cb : _on_destroy) {
		cb(*this);
	}

	delete _workspaces_history;
	free(_app_id);
	free(_title);
}


Window & Window::move(Geo x, Geo y) {
	_x = x;
	_y = y;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_set_size(_xdg_surface->toplevel, _width, _height);
	}
}

Window & Window::resize(Geo width, Geo height) {
	_width = width;
	_height = height;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_set_size(_xdg_surface->toplevel, _width, _height);
	}
}

Window & Window::close() {
	if (_xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_send_close(_xdg_surface->toplevel);
	}
}

Window & Window::maximize() {
	_maximized = !_maximized;

	if (_xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_set_maximized(_xdg_surface->toplevel, _maximized);
	}
	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_maximized(_foreign_toplevel, _maximized);
	// }
}

Window & Window::minimize() {
	_minimized = !_minimized;

	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_minimized(_foreign_toplevel, _minimized);
	// }
}

Window & Window::fullscreen() {
	_fullscreen = !_fullscreen;

	if (_xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_set_fullscreen(_xdg_surface->toplevel, _fullscreen);
	}
	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_fullscreen(_foreign_toplevel, _fullscreen);
	// }
}

Server * Window::server() const {

}

Workspace * Window::workspace() const {

}

WorkspacesHistory * Window::workspace_history() const {

}

const char * Window::title() const {
	return _title;
}

const char * Window::app_id() const {
	return _app_id;
}

Geo Window::x() const {
	return _x;
}

Geo Window::y() const {
	return _y;
}

Geo Window::width() const {
	return _width;
}

Geo Window::height() const {
	return _height;
}

bool Window::mapped() const {
	return _mapped;
}

bool Window::minimized() const {
	return _minimized;
}

bool Window::maximized() const {
	return _maximized;
}

bool Window::fullscreen() const {
	return _fullscreen;
}

void * Window::data() const {
	return _data;
}

struct wlr_xdg_surface * Window::xdg_surface() const {
	return _xdg_surface;
}

struct wlr_xwayland_surface * Window::xwayland_surface() const {
	return _xwayland_surface;
}

struct wlr_surface * Window::surface() const {
	return _surface;
}

Window & Window::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
}

void Window::_handle_destroy(struct wl_listener * listener, void * data) {
	Window * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
