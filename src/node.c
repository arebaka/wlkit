#include "wlkit.h"

struct wlkit_node * wlkit_node_create() {
	struct wlkit_node * node = malloc(sizeof(*node));
	if (!node) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit node");
		return NULL;
	}

	static wlkit_node_id next_id = 0;
	node->id = ++next_id;
	node->user_data = NULL;

	return node;
}

void wlkit_node_destroy(struct wlkit_node * node) {
	if (node) {
		free(node);
	}
}

void wlkit_node_init(struct wlkit_node * node, enum wlkit_node_type type, union wlkit_node_object object) {
	node->type = type;
	node->object = object;
	wl_signal_init(&node->events.destroy);
}

struct wlr_scene_tree * wlkit_alloc_scene_tree(struct wlr_scene_tree * parent, bool * failed) {
	if (*failed) {
		return NULL;
	}

	struct wlr_scene_tree * tree = wlr_scene_tree_create(parent);
	if (!tree) {
		wlr_log(WLR_ERROR, "Unable to allocate a scene node");
		*failed = true;
	}

	return tree;
}
