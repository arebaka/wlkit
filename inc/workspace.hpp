#pragma once

#include <list>

#include "common.hpp"
#include "window.hpp"

namespace wlkit {

class Workspace {
public:
	typedef std::function<void(Workspace&)> Handler;
	typedef uint32_t ID;

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

public:
	Workspace(
		Server * server,
		Layout * layout,
		ID id,
		const char * name);
	~Workspace();

	Server * server() const;
	Layout * layout() const;
	ID id() const;
	const char * name();
	std::list<Window*> windows();
	WindowsHistory * windows_history();
	Window * focused_window();
	void * data();

	// TODO setters
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
