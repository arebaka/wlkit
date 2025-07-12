#include "window.hpp"
#include "workspace.hpp"

#include <wlroots-0.19/wlr/types/wlr_xdg_shell.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>

using namespace wlkit;

Window::Window(struct Server * server, struct Workspace * workspace,
	struct wlr_xdg_surface * xdg_surface, const char * title, const char * app_id):
_server(server), _workspace(workspace), _xdg_surface(xdg_surface), _data(nullptr) {
	_title = strdup(title ? title : "");
	_app_id = strdup(app_id ? app_id : "");
	_workspaces_history = new WorkspacesHistory();
	_mapped = false;
	_surface = xdg_surface->surface;
}

Window::~Window() {
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
	if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_maximized(_foreign_toplevel, _maximized);
	}
}

Window & Window::minimize() {
	_minimized = !_minimized;

	if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_minimized(_foreign_toplevel, _minimized);
	}
}

Window & Window::fullscreen() {
	_fullscreen = !_fullscreen;

	if (_xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_set_fullscreen(_xdg_surface->toplevel, _fullscreen);
	}
	if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_fullscreen(_foreign_toplevel, _fullscreen);
	}
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

struct wlr_surface * Window::wlr_surface() const {
	return _surface;
}

struct wlr_xwayland_surface * Window::xwayland_surface() const {
	return _xwayland_surface;
}

struct wlr_foreign_toplevel_handle_v1 * Window::foreign_toplevel() const {
	return _foreign_toplevel;
}
