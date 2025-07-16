#pragma once

extern "C" {
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
}

#include "common.hpp"

namespace wlkit {

class Cursor {
public:
	using Handler = std::function<void(Cursor*)>;
	using Size = uint32_t;

private:
	Root * _root;

	struct ::wlr_cursor * _wlr_cursor;
	struct ::wlr_xcursor_manager * _xcursor_manager;

	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct ::wl_listener _destroy_listener;

public:
	Cursor(
		Root * root,
		char * name,
		const Size & size,
		const Handler & callback);
	~Cursor();

	[[nodiscard]] Root * root() const;
	[[nodiscard]] struct ::wlr_cursor * wlr_cursor() const;
	[[nodiscard]] struct ::wlr_xcursor_manager * wlr_xcursor_manager() const;
	[[nodiscard]] void * data() const;

	Cursor & set_data(void * data);

	Cursor & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
};

}
