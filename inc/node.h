#ifndef WLKIT_NODE_H
#define WLKIT_NODE_H

#include "common.h"

enum wlkit_node_type {
	WLKIT_NODE_ROOT,
	WLKIT_NODE_OUTPUT,
	WLKIT_NODE_WORKSPACE,
	WLKIT_NODE_CONTAINER,
};

union wlkit_node_object {
	struct wlkit_root * root;
	struct wlkit_output * output;
	struct wlkit_workspace * workspace;
	// wlkit_container * container;
};

struct wlkit_node {
	enum wlkit_node_type type;
	union wlkit_node_object object;

	wlkit_node_id id;

	struct {
		struct wl_signal destroy;
	} events;

	void * user_data;
};

struct wlkit_node * wlkit_node_create();

void wlkit_node_destroy(
	struct wlkit_node * node
);

void wlkit_node_init(
	struct wlkit_node * node,
	enum wlkit_node_type type,
	union wlkit_node_object object
);

struct wlr_scene_tree * wlkit_alloc_scene_tree(
	struct wlr_scene_tree * parent,
	bool * failed
);

#endif // WLKIT_NODE_H
