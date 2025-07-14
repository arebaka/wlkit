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
	using Handler = std::function<void(Window&)>;

private:
	Server * _server;
	Workspace * _workspace;
	struct ::wlr_xdg_surface * _xdg_surface;
	struct wlr_xwayland_surface * _xwayland_surface;  // isnt real wlr_xwayland_surface
	char * _title;
	char * _app_id;

	struct wlr_surface * _surface;
	// struct wlr_foreign_toplevel_handle_v1 * _foreign_toplevel;

	Geo _x, _y, _width, _height;
	bool _mapped, _minimized, _maximized, _fullscreen;
	WorkspacesHistory * _workspaces_history;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	wl_listener _destroy_listener;

public:
	Window(
		struct Server * server,
		struct Workspace * workspace,
		struct wlr_xdg_surface * xdg_surface,
		struct wlr_xwayland_surface * xwayland_surface,
		const char * title,
		const char * app_id,
		const Handler & callback);
	~Window();

	Window & move(Geo x, Geo y);
	Window & resize(Geo width, Geo height);
	Window & close();
	Window & maximize();
	Window & minimize();
	Window & fullscreen();

	[[nodiscard]] Server * server() const;
	[[nodiscard]] Workspace * workspace() const;
	[[nodiscard]] WorkspacesHistory * workspace_history() const;
	[[nodiscard]] const char * title() const;
	[[nodiscard]] const char * app_id() const;
	[[nodiscard]] Geo x() const;
	[[nodiscard]] Geo y() const;
	[[nodiscard]] Geo width() const;
	[[nodiscard]] Geo height() const;
	[[nodiscard]] bool mapped() const;
	[[nodiscard]] bool minimized() const;
	[[nodiscard]] bool maximized() const;
	[[nodiscard]] bool fullscreen() const;
	[[nodiscard]] void * data() const;
	[[nodiscard]] struct wlr_xdg_surface * xdg_surface() const;
	[[nodiscard]] struct wlr_xwayland_surface * xwayland_surface() const;
	[[nodiscard]] struct wlr_surface * surface() const;
	// [[nodiscard]] struct wlr_foreign_toplevel_handle_v1 * foreign_toplevel() const;

	// TODO setters

	Window & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

class WindowsHistory {
private:
	std::list<Window*> _order;
	std::unordered_map<Window*, std::list<Window*>::iterator> _pos;

public:
	WindowsHistory();
	~WindowsHistory();

	const std::list<Window*> & history() const;
	WindowsHistory & shift(Window * workspace);
	WindowsHistory & remove(Window * workspace);
	Window * previous() const;
};

}
