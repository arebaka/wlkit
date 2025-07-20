#pragma once

#include "common.hpp"

namespace wlkit {

class Workspace {
public:
	using Handler = std::function<void(Workspace*)>;
	using ID = uint32_t;

private:
	Server * _server;
	Layout * _layout;

	ID _id;
	char * _name;
	std::list<Window*> _windows;
	WindowsHistory * _windows_history;
	Window * _focused_window;
	Output * _output;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	// std::list<Handler> _on_activate;
	// std::list<Handler> _on_deactivate;
	// std::list<Handler> _on_window_added;
	// std::list<Handler> _on_window_removed;
	// std::list<Handler> _on_layout_change;

	struct ::wl_listener _destroy_listener;

public:
	Workspace(
		Server * server,
		Layout * layout,
		ID id,
		const char * name,
		const Handler & callback);
	~Workspace();

	Workspace & add_window(Window * window);
	Workspace & remove_window(Window * window);
	Workspace & focus_window(Window * window);

	[[nodiscard]] Server * server() const;
	[[nodiscard]] Layout * layout() const;
	[[nodiscard]] ID id() const;
	[[nodiscard]] const char * name() const;
	[[nodiscard]] std::list<Window*> windows() const;
	[[nodiscard]] WindowsHistory * windows_history() const;
	[[nodiscard]] Window * focused_window() const;
	[[nodiscard]] Output * output() const;
	[[nodiscard]] void * data() const;

	Workspace & set_output(Output * output);
	// TODO setters

	Workspace & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
};

class WorkspacesHistory {
public:
	using Iterator = std::list<Workspace*>::iterator;
	using ConstIterator = std::list<Workspace*>::const_iterator;

private:
	std::list<Workspace*> _order;
	std::unordered_map<Workspace*, std::list<Workspace*>::iterator> _pos;

public:
	WorkspacesHistory();
	~WorkspacesHistory();

	const std::list<Workspace*> & history() const;

	WorkspacesHistory & shift(Workspace * workspace);
	WorkspacesHistory & remove(Workspace * workspace);

	Workspace * top() const;
	Workspace * previous() const;

	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;
	ConstIterator cbegin() const;
	ConstIterator cend() const;
};

}
