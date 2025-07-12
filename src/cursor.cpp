#include "cursor.hpp"
#include "root.hpp"

extern "C" {
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
}

using namespace wlkit;

Cursor::Cursor(Root & root, char & name, const Size size):
_root(&root), _data(nullptr) {
	if (!_root || !_root->output_layout()) {
		// TODO error
	}

	_wlr_cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(_wlr_cursor, _root->output_layout());

	_xcursor_manager = wlr_xcursor_manager_create(&name, size);
	wlr_xcursor_manager_load(_xcursor_manager, 1);
}

Cursor::~Cursor() {
	wlr_cursor_destroy(_wlr_cursor);
	wlr_xcursor_manager_destroy(_xcursor_manager);
}

Root * Cursor::root() const {
	return _root;
}

struct wlr_cursor * Cursor::wlr_cursor() const {
	return _wlr_cursor;
}

struct wlr_xcursor_manager * Cursor::wlr_xcursor_manager() const {
	return _xcursor_manager;
}

Cursor & Cursor::set_root(Root * root) {
	_root = root;
	return *this;
}
