#pragma once

#include "common.hpp"

namespace wlkit {

class Workspace {
public:
	using Handler = std::function<void(Workspace&)>;
	using ID = uint32_t;

private:
	Server * _server;
	Layout * _layout;

	ID _id;
	char * _name;
	std::list<Window*> _windows;
	WindowsHistory * _windows_history;
	Window * _focused_window;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	// std::list<Handler> _on_activate;
	// std::list<Handler> _on_deactivate;
	// std::list<Handler> _on_window_added;
	// std::list<Handler> _on_window_removed;
	// std::list<Handler> _on_layout_change;

	wl_listener _destroy_listener;

public:
	Workspace(
		Server * server,
		Layout * layout,
		ID id,
		const char * name,
		const Handler & callback);
	~Workspace();

	[[nodiscard]] Server * server() const;
	[[nodiscard]] Layout * layout() const;
	[[nodiscard]] ID id() const;
	[[nodiscard]] const char * name();
	[[nodiscard]] std::list<Window*> windows();
	[[nodiscard]] WindowsHistory * windows_history();
	[[nodiscard]] Window * focused_window();
	[[nodiscard]] void * data();

	// TODO setters

	Workspace & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

class WorkspacesHistory {
private:
	std::list<Workspace*> _order;
	std::unordered_map<Workspace*, std::list<Workspace*>::iterator> _pos;

public:
	WorkspacesHistory();
	~WorkspacesHistory();

	const std::list<Workspace*> & history() const;
	WorkspacesHistory & shift(Workspace * workspace);
	WorkspacesHistory & remove(Workspace * workspace);
	Workspace * previous() const;
};

}
