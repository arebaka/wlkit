#include "workspace.hpp"
#include "server.hpp"
#include "window.hpp"

#include <algorithm>

using namespace wlkit;

Workspace::Workspace(Server * server, Layout * layout, ID id, const char * name, const Handler & callback):
_server(server), _layout(layout), _id(id), _focused_window(nullptr), _data(nullptr) {
	_name = strdup(name ? name : "");
	_windows_history = new WindowsHistory();

	_server->add_workspace(this);

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Workspace::~Workspace() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	free(_name);
}

Workspace & Workspace::add_window(Window * window) {
	_windows.push_back(window);
	return *this;
}

Workspace & Workspace::remove_window(Window * window) {
	if (window == _focused_window) {
		_focused_window = _windows_history->previous();
	}

	_windows.remove(window);
	_windows_history->remove(window);

	return *this;
}

Workspace & Workspace::focus_window(Window * window) {
	if (!window) {
		return *this;
	}
	if (std::find(_windows.begin(), _windows.end(), window) == _windows.end()) {
		return *this;
	}

	_focused_window = window;
	_windows_history->shift(window);

	return *this;
}

Server * Workspace::server() const {
	return _server;
}

Layout * Workspace::layout() const {
	return _layout;
}

Workspace::ID Workspace::id() const {
	return _id;
}

const char * Workspace::name() const {
	return _name;
}

std::list<Window*> Workspace::windows() const {
	return _windows;
}

WindowsHistory * Workspace::windows_history() const {
	return _windows_history;
}

Window * Workspace::focused_window() const {
	return _focused_window;
}

void * Workspace::data() const {
	return _data;
}

Workspace & Workspace::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

void Workspace::_handle_destroy(struct wl_listener * listener, void * data) {
	Workspace * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
