#include "window.hpp"
#include "workspace.hpp"
#include "server.hpp"
#include "root.hpp"
#include "node.hpp"

using namespace wlkit;

struct PendingXDG {
	struct wl_listener listener;
	Window * window;
	const char * title;
	const char * app_id;
};

Window::Window(Server * server, Workspace * workspace,
	struct wlr_xdg_surface * xdg_surface, struct wlr_xwayland_surface * xwayland_surface,
	const char * title, const char * app_id, const Handler & callback):
_server(server), _workspace(workspace),
_xdg_surface(xdg_surface), _xwayland_surface(xwayland_surface),
_x(0.0), _y(0.0), _width(1.0), _height(1.0),
_mapped(false), _minimized(false), _maximized(false), _fullscreened(false),
_ready(false), _dirty(true), _resizing(false), _data(nullptr) {
	_workspaces_history = new WorkspacesHistory();

	if (_xdg_surface) {
		if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
			_title = strdup(title ? title : _xdg_surface->toplevel->title ? _xdg_surface->toplevel->title : "");
			_app_id = strdup(app_id ? app_id : _xdg_surface->toplevel->app_id ? _xdg_surface->toplevel->app_id : "");
			_setup_xdg();
		}
		// TODO popup
	}
	else {
		_title = strdup(title ? title : "");
		_app_id = strdup(app_id ? app_id : "");

		_server->add_window(this);
		if (_workspace) {
			_workspace->add_window(this);
			_workspaces_history->shift(_workspace);
		}

		_ready = true;
	}

	// _foreign_toplevel = wlr_foreign_toplevel_handle_v1_create(_server->foreign_toplevel_manager_v1());
	// wlr_foreign_toplevel_handle_v1_set_title(_foreign_toplevel, _title);
	// wlr_foreign_toplevel_handle_v1_set_app_id(_foreign_toplevel, _app_id);
	// wl_signal_add(&_foreign_toplevel->events.destroy, &_destroy_listener);

	// _request_close_listener.notify = _handle_request_close;
	// wl_signal_add(&_foreign_toplevel->events.request_close, &_request_close_listener);
	// TODO handlers

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Window::~Window() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	close();

	delete _workspaces_history;
	free(_app_id);
	free(_title);
}

Window & Window::close() {
	if (_workspace) {
		_workspace->remove_window(this);
	}
	_workspace = nullptr;

	if (_xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		// wlr_xdg_toplevel_send_close(_xdg_surface->toplevel);
	}

	return *this;
}

Window & Window::move(Geo x, Geo y) {
	_x = x;
	_y = y;
	_dirty = true;

	for (auto & cb : _on_move) {
		cb(this);
	}

	return *this;
}

Window & Window::resize(Geo width, Geo height) {
	if (width < 1 || height < 1) {
		return *this;
	}

	_width = width;
	_height = height;
	_dirty = true;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_size(_xdg_surface->toplevel, _width, _height);
	}

	for (auto & cb : _on_resize) {
		cb(this);
	}

	return *this;
}

Window & Window::map() {
	_mapped = true;
	return *this;
}

Window & Window::unmap() {
	_mapped = false;
	return *this;
}

Window & Window::maximize() {
	if (_maximized) {
		return *this;
	}

 	_maximized = true;
	_dirty = true;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_maximized(_xdg_surface->toplevel, _maximized);
	}
	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_maximized(_foreign_toplevel, _maximized);
	// }

	return *this;
}

Window & Window::unmaximize() {
	if (!_maximized) {
		return *this;
	}

	_maximized = false;
	_dirty = true;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_maximized(_xdg_surface->toplevel, _maximized);
	}

	return *this;
}

Window & Window::minimize() {
	if (_minimized) {
		return *this;
	}

	_minimized = true;
	_dirty = true;

	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_minimized(_foreign_toplevel, _minimized);
	// }

	return *this;
}

Window & Window::unminimize() {
	if (!_minimized) {
		return *this;
	}

	_minimized = false;
	_dirty = true;

	return *this;
}

Window & Window::fullscreen() {
	if (_fullscreened) {
		return *this;
	}

	_fullscreened = true;
	_dirty = true;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_fullscreen(_xdg_surface->toplevel, _fullscreened);
	}
	// if (_foreign_toplevel) {
		// wlr_foreign_toplevel_handle_v1_set_fullscreen(_foreign_toplevel, _fullscreen);
	// }

	return *this;
}

Window & Window::unfullscreen() {
	if (!_fullscreened) {
		return *this;
	}

	_fullscreened = false;
	_dirty = true;

	if (_xdg_surface && _xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
		wlr_xdg_toplevel_set_fullscreen(_xdg_surface->toplevel, _fullscreened);
	}

	return *this;
}

Server * Window::server() const {
	return _server;
}

Workspace * Window::workspace() const {
	return _workspace;
}

struct wlr_xdg_surface * Window::xdg_surface() const {
	return _xdg_surface;
}

struct wlr_xwayland_surface * Window::xwayland_surface() const {
	return _xwayland_surface;
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

bool Window::fullscreened() const {
	return _fullscreened;
}

bool Window::ready() const {
	return _ready;
}

bool Window::dirty() const {
	return _dirty;
}

WorkspacesHistory * Window::workspaces_history() const {
	return _workspaces_history;
}

void * Window::data() const {
	return _data;
}

Window & Window::drawn() {
	_dirty = false;
	return *this;
}

Window & Window::set_workspace(Workspace * workspace) {
	if (_workspace) {
		_workspace->remove_window(this);
	}

	_workspace = workspace;
	_workspaces_history->shift(_workspace);
	workspace->add_window(this);

	return *this;
}

Window & Window::set_title(const char * title) {
	free(_title);
	_title = strdup(title ? title : "");

	for (auto & cb : _on_set_title) {
		cb(this);
	}

	return *this;
}

Window & Window::set_app_id(const char * app_id) {
	free(_app_id);
	_app_id = strdup(app_id ? app_id : "");

	for (auto & cb : _on_set_app_id) {
		cb(this);
	}

	return *this;
}

Window & Window::set_data(void * data) {
	_data = data;
	return *this;
}

Window & Window::set_xdg_surface(struct wlr_xdg_surface * xdg_surface) {
	_xdg_surface = xdg_surface;
	return *this;
}

Window & Window::set_xwayland_surface(struct wlr_xwayland_surface * xwayland_surface) {
	_xwayland_surface = xwayland_surface;
	return *this;
}

Window & Window::on_set_title(const Handler & handler) {
	if (handler) {
		_on_set_title.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_set_app_id(const Handler & handler) {
	if (handler) {
		_on_set_app_id.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_move(const Handler & handler) {
	if (handler) {
		_on_move.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_resize(const Handler & handler) {
	if (handler) {
		_on_resize.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_map(const Handler & handler) {
	if (handler) {
		_on_map.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_unmap(const Handler & handler) {
	if (handler) {
		_on_unmap.push_back(std::move(handler));
	}
	return *this;
}

void Window::_setup_xdg() {
	bool failed = false;
	auto scene = Node::alloc_scene_tree(_server->root()->staging(), &failed);
	if (!failed) {
		wlr_scene_xdg_surface_create(scene, _xdg_surface);
	}
	// xdg_shell_view->image_capture_tree =
	// 	wlr_scene_xdg_surface_create(&xdg_shell_view->view.image_capture_scene->tree, xdg_toplevel->base);

	_server->add_window(this);
	if (_workspace) {
		_workspace->add_window(this);
		_workspaces_history->shift(_workspace);
	}

	_set_title_listener.notify = _handle_set_title;
	wl_signal_add(&_xdg_surface->toplevel->events.set_title, &_set_title_listener);
	_set_app_id_listener.notify = _handle_set_app_id;
	wl_signal_add(&_xdg_surface->toplevel->events.set_app_id, &_set_app_id_listener);
	_map_listener.notify = _handle_map;
	wl_signal_add(&_xdg_surface->surface->events.map, &_map_listener);
	_unmap_listener.notify = _handle_unmap;
	wl_signal_add(&_xdg_surface->surface->events.unmap, &_unmap_listener);
	_configure_listener.notify = _handle_configure;
	wl_signal_add(&_xdg_surface->events.configure, &_configure_listener);
	_ack_configure_listener.notify = _handle_ack_configure;
	wl_signal_add(&_xdg_surface->events.ack_configure, &_ack_configure_listener);
	_commit_listener.notify = _handle_commit;
	wl_signal_add(&_xdg_surface->surface->events.commit, &_commit_listener);
	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_xdg_surface->surface->events.destroy, &_destroy_listener);
	wl_signal_add(&_xdg_surface->events.destroy, &_destroy_listener);

	_ready = false;
}

void Window::_configure_xdg() {
	if (!_xdg_surface ||
		!_xdg_surface->initialized ||
		_xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL
	) {
		return;
	}

	wlr_xdg_toplevel_set_size(_xdg_surface->toplevel, _width, _height);
	wlr_xdg_toplevel_set_maximized(_xdg_surface->toplevel, _maximized);
	wlr_xdg_toplevel_set_fullscreen(_xdg_surface->toplevel, _fullscreened);
	wlr_xdg_toplevel_set_activated(_xdg_surface->toplevel, true);

	_ready = true;
}

void Window::_handle_set_title(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _set_title_listener);
	window->set_title(window->_xdg_surface->toplevel->title);
}

void Window::_handle_set_app_id(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _set_app_id_listener);
	window->set_app_id(window->_xdg_surface->toplevel->app_id);
}

void Window::_handle_map(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _map_listener);
	window->_mapped = true;

	for (auto & cb : window->_on_map) {
		cb(window);
	}
}

void Window::_handle_unmap(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _unmap_listener);
	window->_mapped = false;

	for (auto & cb : window->_on_unmap) {
		cb(window);
	}
}

void Window::_handle_configure(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _configure_listener);
	auto event = static_cast<struct wlr_xdg_surface_configure*>(data);
	auto surface = window->_xdg_surface;

	if (window->_xdg_surface) {
		if (surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
			if (event->toplevel_configure->width > 0 && event->toplevel_configure->height > 0) {
				window->_width = event->toplevel_configure->width;
				window->_height = event->toplevel_configure->height;
			}

			window->_maximized = event->toplevel_configure->maximized;
			window->_fullscreened = event->toplevel_configure->fullscreen;
			window->_resizing = event->toplevel_configure->resizing;
		}
	}
}

void Window::_handle_ack_configure(struct wl_listener * listener, void * data) {

}

void Window::_handle_commit(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _commit_listener);
	if (!window->_ready) {
		if (window->_xdg_surface) {
			window->_configure_xdg();
		}
	}
}

void Window::_handle_destroy(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _destroy_listener);
	delete window;
}
