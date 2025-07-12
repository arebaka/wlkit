#pragma once

#include <list>

#include "common.hpp"

#include <wlr/types/wlr_foreign_toplevel_management_v1.h>

namespace wlkit {

class Window {
public:
	typedef std::function<void(Window&)> Handler;

private:
	Server * _server;
	Workspace * _workspace;
	WorkspacesHistory * _workspaces_history;

	char * _title;
	char * _app_id;

	Geo _x, _y, _width, _height;
	bool _mapped, _minimized, _maximized, _fullscreen;
	void * _data;

	struct wlr_xdg_surface * _xdg_surface;
	struct wlr_surface * _surface;
	struct wlr_xwayland_surface * _xwayland_surface;
	struct wlr_foreign_toplevel_handle_v1 * _foreign_toplevel;

public:
	Window(
		struct Server * server,
		struct Workspace * workspace,
		struct wlr_xdg_surface * xdg_surface,
		const char * title,
		const char * app_id);
	~Window();

	Window & move(Geo x, Geo y);
	Window & resize(Geo width, Geo height);
	Window & close();
	Window & maximize();
	Window & minimize();
	Window & fullscreen();

	Server * server() const;
	Workspace * workspace() const;
	WorkspacesHistory * workspace_history() const;
	const char * title() const;
	const char * app_id() const;
	Geo x() const;
	Geo y() const;
	Geo width() const;
	Geo height() const;
	bool mapped() const;
	bool minimized() const;
	bool maximized() const;
	bool fullscreen() const;
	void * data() const;
	struct wlr_xdg_surface * xdg_surface() const;
	struct wlr_surface * wlr_surface() const;
	struct wlr_xwayland_surface * xwayland_surface() const;
	struct wlr_foreign_toplevel_handle_v1 * foreign_toplevel() const;

	// TODO setters
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
