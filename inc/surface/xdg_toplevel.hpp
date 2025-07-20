#pragma once

extern "C" {
#include <wlr/types/wlr_xdg_shell.h>
}

#include "../surface.hpp"

namespace wlkit {

class XDGToplevel : public Surface {
private:
	struct ::wlr_xdg_surface * _xdg_surface;
	struct ::wlr_xdg_toplevel * _toplevel;

public:
	XDGToplevel(struct ::wlr_xdg_surface * surface);
	~XDGToplevel();

	bool is_xdg_toplevel() const;
	XDGToplevel * as_xdg_toplevel();
	bool is_xdg_popup() const;
	XDGPopup * as_xdg_popup();
	bool is_xwayland() const;
	XWayland * as_xwayland();

	bool initialized() const;
	const char * title() const;
	const char * app_id() const;
	void ping();
	void close();
	Serial set_size(Geo width, Geo height);
	Serial set_maximized(bool maximized);
	Serial set_fullscreen(bool fullscreen);
	Serial set_activated(bool activated);

	struct ::wlr_xdg_surface * xdg_surface();
	struct ::wlr_xdg_toplevel * toplevel();
};

}
