#ifndef WLKIT_ROOT_H
#define WLKIT_ROOT_H

#include "common.h"

#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>

struct wlkit_root {
	struct wlkit_server * server;
	struct wlkit_node * node;

	struct wlr_output_layout * output_layout;
	struct wlr_scene * scene;
	struct wlr_scene_tree * staging;
	struct wlr_scene_tree * layer_tree;

	wlkit_geo_t x, y, width, height;

	struct wlkit_cursor * cursor;

	struct wl_list layers;
	struct wl_list outputs;

	struct {
		struct wl_signal new_node;
	} events;

	void * user_data;
};

struct wlkit_root * wlkit_root_create(
	struct wlkit_server * server
);

void wlkit_root_destroy(
	struct wlkit_root * root
);

#endif // WLKIT_ROOT_H
