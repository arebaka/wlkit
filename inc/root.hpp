#pragma once

extern "C" {
#include <wlr/types/wlr_output_layout.h>
}

#include "common.hpp"
#include "cursor.hpp"

namespace wlkit {

class Root {
public:
	typedef std::function<void(Root&)> Handler;

private:
	Server * _server;

	struct ::wlr_scene * _scene;
	struct ::wlr_output_layout * _output_layout;
	struct ::wlr_scene_tree * _staging;
	struct ::wlr_scene_tree * _layer_tree;

	Geo _x, _y, _width, _height;
	Node * _node;
	Cursor * _cursor;
	void * _data;

	// struct wl_list layers;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct wl_listener _destroy_listener;

public:
	Root(
		Server * server,
		char * cursor_name,
		const Cursor::Size & cursor_size,
		const Handler & callback);
	~Root();

	[[nodiscard]] struct wlr_scene * scene() const;
	[[nodiscard]] struct wlr_output_layout * output_layout() const;

	Root & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

}
