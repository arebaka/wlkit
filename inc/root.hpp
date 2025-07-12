#pragma once

#include <list>

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

	struct wlr_scene * _scene;
	struct wlr_output_layout * _output_layout;
	struct wlr_scene_tree * _staging;
	struct wlr_scene_tree * _layer_tree;

	Geo _x, _y, _width, _height;
	Node * _node;
	Cursor * _cursor;

	void * _data;

	// struct wl_list layers;

	struct wl_signal _new_node_event;

public:
	Root(
		Server & server,
		char * cursor_name,
		const Cursor::Size & cursor_size);
	~Root();

	struct wlr_scene * scene();
	struct wlr_output_layout * output_layout();
};

}
