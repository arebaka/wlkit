#include "workspace.hpp"
#include "server.hpp"

using namespace wlkit;

Workspace::Workspace(Server & server, Layout & layout, ID id, const char * name, const Handler & callback):
_server(&server), _layout(&layout), _id(id) {
	_name = strdup(name ? name : "");
	_focused_window = nullptr;
	_data = nullptr;

	// _layout->workspace = this;
	_server->add_workspace(this);

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(*this);
	}
}

Workspace::~Workspace() {
	for (auto & cb : _on_destroy) {
		cb(*this);
	}

	free(_name);
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

const char * Workspace::name() {
	return _name;
}

std::list<Window*> Workspace::windows() {
	return _windows;
}

Window * Workspace::focused_window() {
	return _focused_window;
}

Workspace & Workspace::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
}

void Workspace::_handle_destroy(struct wl_listener * listener, void * data) {
	Workspace * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
