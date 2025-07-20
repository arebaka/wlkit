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
	~XDGToplevel() override;

	bool is_xdg_toplevel() const override;
	XDGToplevel * as_xdg_toplevel() override;

	bool initialized() const override;
	const char * title() const override;
	const char * app_id() const override;
	void ping() override;
	void close() override;
	Serial set_size(Geo width, Geo height) override;
	Serial set_maximized(bool maximized) override;
	Serial set_fullscreen(bool fullscreen) override;
	Serial set_activated(bool activated) override;

	struct ::wlr_xdg_surface * xdg_surface();
	struct ::wlr_xdg_toplevel * toplevel();
};

}
