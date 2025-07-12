#include "workspace.hpp"
#include "server.hpp"

using namespace wlkit;

Workspace::Workspace(Server * server, Layout * layout, ID id, const char * name):
_server(server), _layout(layout), _id(id) {
	_name = strdup(name ? name : "");
	_focused_window = nullptr;
	_data = nullptr;

	// _layout->workspace = this;
	server->add_workspace(this);

	for (auto & cb : _on_create) {
		cb(*this);
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
