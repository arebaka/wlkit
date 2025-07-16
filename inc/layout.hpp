#pragma once

#include "common.hpp"

namespace wlkit {

class Layout {
public:
	using Handler = std::function<void(Layout*)>;

private:
	char * _name;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct ::wl_listener _destroy_listener;

public:
	Layout(
		const char * name,
		const Handler & callback);
	~Layout();

	// Layout & arrange(Workspace * workspace);
	// Layout & handle_new_window(Workspace * workspace, Window * window);
	// Layout & handle_window_removed(Workspace * workspace, Window * window);
	// Layout & handle_window_focus(Workspace * workspace, Window * window);

	[[nodiscard]] const char * name() const;
	[[nodiscard]] void * data() const;

	Layout & set_data(void * data);

	Layout & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
};

}
