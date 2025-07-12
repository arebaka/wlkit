#pragma once

extern "C" {
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
}

#include "common.hpp"

namespace wlkit {

class Cursor {
public:
	typedef std::function<void(Cursor&)> Handler;
	typedef uint32_t Size;

private:
	Root * _root;

	struct wlr_cursor * _wlr_cursor;
	struct wlr_xcursor_manager * _xcursor_manager;

	void * _data;

public:
	Cursor(
		Root & root,
		char & name,
		const Size size);
	~Cursor();

	Root * root() const;
	struct wlr_cursor * wlr_cursor() const;
	struct wlr_xcursor_manager * wlr_xcursor_manager() const;

	Cursor & set_root(Root * root);
};

}
