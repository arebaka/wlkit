#pragma once

extern "C" {
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>
// #pragma push_macro("class")
// #undef class
// #define class class_field
// #include <wlr/xwayland/xwayland.h>
// #undef class
// #pragma pop_macro("class")
}

#include "common.hpp"

namespace wlkit {

class Window {
public:
	using Handler = std::function<void(Window*)>;

private:
	Server * _server;
	Workspace * _workspace;
	struct ::wlr_xdg_surface * _xdg_surface;
	struct wlr_xwayland_surface * _xwayland_surface;  // isnt real wlr_xwayland_surface
	char * _title;
	char * _app_id;

	// struct ::wlr_foreign_toplevel_handle_v1 * _foreign_toplevel;

	Geo _x, _y, _width, _height;
	bool _mapped, _minimized, _maximized, _fullscreened;
	bool _ready, _dirty, _resizing;
	WorkspacesHistory * _workspaces_history;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_set_title;
	std::list<Handler> _on_set_app_id;
	std::list<Handler> _on_destroy;
	std::list<Handler> _on_move;
	std::list<Handler> _on_resize;
	std::list<Handler> _on_map;
	std::list<Handler> _on_unmap;

	struct ::wl_listener _set_title_listener;
	struct ::wl_listener _set_app_id_listener;
	struct ::wl_listener _map_listener;
	struct ::wl_listener _unmap_listener;
	struct ::wl_listener _configure_listener;
	struct ::wl_listener _ack_configure_listener;
	struct ::wl_listener _commit_listener;
	struct ::wl_listener _destroy_listener;

public:
	Window(
		struct Server * server,
		struct Workspace * workspace,
		struct ::wlr_xdg_surface * xdg_surface,
		struct wlr_xwayland_surface * xwayland_surface,  // isnt real wlr_xwayland_surface
		const char * title,
		const char * app_id,
		const Handler & callback);
	~Window();

	Window & close();
	Window & move(Geo x, Geo y);
	Window & resize(Geo width, Geo height);
	Window & map();
	Window & unmap();
	Window & maximize();
	Window & unmaximize();
	Window & minimize();
	Window & unminimize();
	Window & fullscreen();
	Window & unfullscreen();

	[[nodiscard]] Server * server() const;
	[[nodiscard]] Workspace * workspace() const;
	[[nodiscard]] struct ::wlr_xdg_surface * xdg_surface() const;
	[[nodiscard]] struct wlr_xwayland_surface * xwayland_surface() const;  // isnt real wlr_xwayland_surface
	[[nodiscard]] const char * title() const;
	[[nodiscard]] const char * app_id() const;
	[[nodiscard]] Geo x() const;
	[[nodiscard]] Geo y() const;
	[[nodiscard]] Geo width() const;
	[[nodiscard]] Geo height() const;
	[[nodiscard]] bool mapped() const;
	[[nodiscard]] bool minimized() const;
	[[nodiscard]] bool maximized() const;
	[[nodiscard]] bool fullscreened() const;
	[[nodiscard]] bool ready() const;
	[[nodiscard]] bool dirty() const;
	[[nodiscard]] WorkspacesHistory * workspaces_history() const;
	[[nodiscard]] void * data() const;
	// [[nodiscard]] struct ::wlr_foreign_toplevel_handle_v1 * foreign_toplevel() const;

	Window & drawn();
	Window & set_workspace(Workspace * workspace);
	Window & set_title(const char * title);
	Window & set_app_id(const char * app_id);
	Window & set_data(void * data);
	Window & set_xdg_surface(struct ::wlr_xdg_surface * xdg_surface);
	Window & set_xwayland_surface(struct wlr_xwayland_surface * xwayland_surface);  // isnt real wlr_xwayland_surface

	Window & on_set_title(const Handler & handler);
	Window & on_set_app_id(const Handler & handler);
	Window & on_destroy(const Handler & handler);
	Window & on_move(const Handler & handler);
	Window & on_resize(const Handler & handler);
	Window & on_map(const Handler & handler);
	Window & on_unmap(const Handler & handler);

private:
	void _setup_xdg();
	void _configure_xdg();

	static void _handle_set_title(struct ::wl_listener * listener, void * data);
	static void _handle_set_app_id(struct ::wl_listener * listener, void * data);
	static void _handle_map(struct ::wl_listener * listener, void * data);
	static void _handle_unmap(struct ::wl_listener * listener, void * data);
	static void _handle_configure(struct ::wl_listener * listener, void * data);
	static void _handle_ack_configure(struct ::wl_listener * listener, void * data);
	static void _handle_commit(struct ::wl_listener * listener, void * data);
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
};

class WindowsHistory {
public:
	using Iterator = std::list<Window*>::iterator;
	using ConstIterator = std::list<Window*>::const_iterator;

private:
	std::list<Window*> _order;
	std::unordered_map<Window*, std::list<Window*>::iterator> _pos;

public:
	WindowsHistory();
	~WindowsHistory();

	const std::list<Window*> & history() const;

	WindowsHistory & shift(Window * workspace);
	WindowsHistory & remove(Window * workspace);

	Window * top() const;
	Window * previous() const;

	Iterator begin();
	Iterator end();
	ConstIterator begin() const;
	ConstIterator end() const;
	ConstIterator cbegin() const;
	ConstIterator cend() const;
};

}
