#include "surface/xdg_toplevel.hpp"

using namespace wlkit;

XDGToplevel::XDGToplevel(struct wlr_xdg_surface * surface):
Surface(surface->surface),
_xdg_surface(surface) {
	_toplevel = wlr_xdg_toplevel_try_from_wlr_surface(surface->surface);
	if (!_toplevel) {
		// TODO error
	}
}

XDGToplevel::~XDGToplevel() {}

bool XDGToplevel::is_xdg_toplevel() const {
	return true;
}

XDGToplevel * XDGToplevel::as_xdg_toplevel() {
	return this;
}

bool XDGToplevel::initialized() const {
	return _xdg_surface->initialized;
}

const char * XDGToplevel::title() const {
	return _toplevel->title;
}

const char * XDGToplevel::app_id() const {
	auto toplevel = wlr_xdg_toplevel_try_from_wlr_surface(_surface);
	return _toplevel->app_id;
}

void XDGToplevel::ping() {
	wlr_xdg_surface_ping(_xdg_surface);
}

void XDGToplevel::close() {
	wlr_xdg_toplevel_send_close(_toplevel);
}

Surface::Serial XDGToplevel::set_size(Geo width, Geo height) {
	return wlr_xdg_toplevel_set_size(_toplevel, width, height);
}

Surface::Serial XDGToplevel::set_maximized(bool maximized) {
	return wlr_xdg_toplevel_set_maximized(_toplevel, maximized);
}

Surface::Serial XDGToplevel::set_fullscreen(bool fullscreen) {
	return wlr_xdg_toplevel_set_fullscreen(_toplevel, fullscreen);
}

Surface::Serial XDGToplevel::set_activated(bool activated) {
	return wlr_xdg_toplevel_set_activated(_toplevel, activated);
}

struct wlr_xdg_surface * XDGToplevel::xdg_surface() {
	return _xdg_surface;
}

struct wlr_xdg_toplevel * XDGToplevel::toplevel() {
	return _toplevel;
}
