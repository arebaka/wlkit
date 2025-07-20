#include "window.hpp"
#include "workspace.hpp"
#include "server.hpp"
#include "root.hpp"
#include "node.hpp"
#include "output.hpp"

#include "surface/xdg_toplevel.hpp"
#include <wayland-util.h>

using namespace wlkit;

Window::Window(Server * server, Workspace * workspace, Surface * surface,
	const char * title, const char * app_id, const Handler & callback):
_server(server), _workspace(workspace), _surface(surface),
_x(0.0), _y(0.0), _width(1.0), _height(1.0),
_mapped(false), _minimized(false), _maximized(false), _fullscreened(false),
_ready(false), _dirty(true), _resizing(false), _closed(false), _data(nullptr) {
	_x = _y = 0.0;
	if (workspace && workspace->output()) {
		auto output = workspace->output();
		_width = output->width();
		_height = output->height();
	} else {
		_width = _height = 1.0;
	}

	_workspaces_history = new WorkspacesHistory();

	if (surface) {
		_title = strdup(title ? title : surface->title() ? surface->title() : "");
		_app_id = strdup(app_id ? app_id : surface->app_id() ? surface->app_id() : "");

		if (surface->is_xdg_toplevel()) {
			_setup_xdg_toplevel();
		}
		// TODO popup
		// TODO xwayland
	}
	else {
		_title = strdup(title ? title : "");
		_app_id = strdup(app_id ? app_id : "");

		_server->add_window(this);
		if (workspace) {
			workspace->add_window(this);
			_workspaces_history->shift(workspace);
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

	if (!_closed) {
		close();
	}

	delete _workspaces_history;
	free(_app_id);
	free(_title);
}

Window & Window::close() {
	if (_workspace) {
		_workspace->remove_window(this);
	}
	_workspace = nullptr;

	if (_server) {
		_server->remove_window(this);
	}
	_server = nullptr;

	if (_surface) {
		_surface->close();
	}

	for (auto & cb : _on_close) {
		cb(this);
	}

	_closed = true;
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

	if (_surface) {
		_surface->set_size(width, height);
	} else {
		_width = width;
		_height = height;
		_dirty = true;
	}

	for (auto & cb : _on_resize) {
		cb(this);
	}

	return *this;
}

/**
 * @deprecated
 */
Window & Window::map() {
	_mapped = true;
	return *this;
}

/**
 * @deprecated
 */
Window & Window::unmap() {
	_mapped = false;
	return *this;
}

Window & Window::maximize() {
	if (_maximized) {
		return *this;
	}

	if (_surface) {
		_surface->set_maximized(_maximized);
	} else {
		_maximized = true;
		_dirty = true;
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

	if (_surface) {
		_surface->set_maximized(_maximized);
	} else {
		_maximized = false;
		_dirty = true;
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

	if (_surface) {
		_surface->set_fullscreen(_fullscreened);
	} else {
		_fullscreened = true;
		_dirty = true;
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

	if (_surface) {
		_surface->set_fullscreen(_fullscreened);
	} else {
		_fullscreened = false;
		_dirty = true;
	}

	return *this;
}

Server * Window::server() const {
	return _server;
}

Workspace * Window::workspace() const {
	return _workspace;
}

Surface * Window::surface() const {
	return _surface;
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

Window & Window::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_close(const Handler & handler) {
	if (handler) {
		_on_close.push_back(std::move(handler));
	}
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

Window & Window::on_configure(const Handler & handler) {
	if (handler) {
		_on_configure.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_ack_configure(const Handler & handler) {
	if (handler) {
		_on_ack_configure.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_commit(const Handler & handler) {
	if (handler) {
		_on_commit.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_ping_timeout(const Handler & handler) {
	if (handler) {
		_on_ping_timeout.push_back(std::move(handler));
	}
	return *this;
}

Window & Window::on_new_subsurface(const NewSubsurfaceHandler & handler) {
	if (handler) {
		_on_new_subsurface.push_back(std::move(handler));
	}
	return *this;
}

void Window::_setup_xdg_toplevel() {
	if (!_surface->is_xdg_toplevel()) {
		// TODO error
	}
	auto surface = _surface->as_xdg_toplevel();
	auto wlr_surface = surface->wlr_surface();
	auto xdg_surface = surface->xdg_surface();
	auto toplevel = surface->toplevel();

	bool failed = false;
	auto scene = Node::alloc_scene_tree(_server->root()->staging(), &failed);
	if (!failed) {
		wlr_scene_xdg_surface_create(scene, xdg_surface);
	}
	// xdg_shell_view->image_capture_tree =
	// 	wlr_scene_xdg_surface_create(&xdg_shell_view->view.image_capture_scene->tree, xdg_toplevel->base);

	_server->add_window(this);
	if (_workspace) {
		_workspace->add_window(this);
		_workspaces_history->shift(_workspace);
	}

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&wlr_surface->events.destroy, &_destroy_listener);
	wl_signal_add(&xdg_surface->events.destroy, &_destroy_listener);
	wl_signal_add(&toplevel->events.destroy, &_destroy_listener);

	_set_title_listener.notify = _handle_set_title;
	wl_signal_add(&toplevel->events.set_title, &_set_title_listener);
	_set_app_id_listener.notify = _handle_set_app_id;
	wl_signal_add(&toplevel->events.set_app_id, &_set_app_id_listener);
	_map_listener.notify = _handle_map;
	wl_signal_add(&wlr_surface->events.map, &_map_listener);
	_unmap_listener.notify = _handle_unmap;
	wl_signal_add(&wlr_surface->events.unmap, &_unmap_listener);
	_configure_listener.notify = _handle_configure;
	wl_signal_add(&xdg_surface->events.configure, &_configure_listener);
	_ack_configure_listener.notify = _handle_ack_configure;
	wl_signal_add(&xdg_surface->events.ack_configure, &_ack_configure_listener);
	_commit_listener.notify = _handle_commit;
	wl_signal_add(&wlr_surface->events.commit, &_commit_listener);
	_ping_timeout_listener.notify = _handle_ping_timeout;
	wl_signal_add(&xdg_surface->events.ping_timeout, &_ping_timeout_listener);
	_new_subsurface_listener.notify = _handle_new_subsurface;
	wl_signal_add(&wlr_surface->events.new_subsurface, &_new_subsurface_listener);

	_ready = false;
}

void Window::_configure_xdg_toplevel() {
	if (!_surface->is_xdg_toplevel()) {
		// TODO error
	}

	if (!_surface->initialized()) {
		return;
	}

	_surface->set_size(_width, _height);
	_surface->set_maximized(_maximized);
	_surface->set_fullscreen(_fullscreened);
	_surface->set_activated(true);

	_ready = true;
}

void Window::_handle_destroy(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _destroy_listener);
	delete window;
}

void Window::_handle_set_title(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _set_title_listener);
	window->set_title(window->_surface->title());
}

void Window::_handle_set_app_id(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _set_app_id_listener);
	window->set_app_id(window->_surface->app_id());
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
	auto surface = window->_surface;

	if (surface->is_xdg_toplevel()) {
		if (event->toplevel_configure->width >= 1 && event->toplevel_configure->height >= 1) {
			window->_width = event->toplevel_configure->width;
			window->_height = event->toplevel_configure->height;
		}

		window->_maximized = event->toplevel_configure->maximized;
		window->_fullscreened = event->toplevel_configure->fullscreen;
		window->_resizing = event->toplevel_configure->resizing;
	}
	// TODO popup
	// TODO xwayland

	window->_dirty = true;

	for (auto & cb : window->_on_configure) {
		cb(window);
	}
}

void Window::_handle_ack_configure(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _ack_configure_listener);

	for (auto & cb : window->_on_ack_configure) {
		cb(window);
	}
}

void Window::_handle_commit(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _commit_listener);
	if (!window->_ready) {
		if (window->_surface->is_xdg_toplevel()) {
			window->_configure_xdg_toplevel();
		}
		// TODO popup
		// TODO xwayland
	}
	// window->_dirty = true;

	for (auto & cb : window->_on_commit) {
		cb(window);
	}
}

void Window::_handle_ping_timeout(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _ping_timeout_listener);

	for (auto & cb : window->_on_ping_timeout) {
		cb(window);
	}
}

void Window::_handle_new_subsurface(struct wl_listener * listener, void * data) {
	Window * window = wl_container_of(listener, window, _new_subsurface_listener);
	auto subsurface = static_cast<struct wlr_subsurface*>(data);

	for (auto & cb : window->_on_new_subsurface) {
		cb(window, subsurface);
	}
}
