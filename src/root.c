#include "wlkit.h"
#include <wayland-util.h>
#include <wlkit/wlkit.h>

struct wlkit_root * wlkit_root_create(struct wlkit_server * server) {
	struct wlkit_root * root = malloc(sizeof(*root));
	if (!root) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit root");
		return NULL;
	}

	root->scene = wlr_scene_create();
	if (!root->scene) {
		wlr_log(WLR_ERROR, "Unable to allocate slkit root scene");
		free(root);
		return NULL;
	}

	root->server = server;

	root->node = wlkit_node_create();
	wlkit_node_init(root->node, WLKIT_NODE_ROOT, (union wlkit_node_object) root);

	root->output_layout = wlr_output_layout_create(server->display);
	if (!root->output_layout) {
		wlr_log(WLR_ERROR, "Failed to create wlkit root output layout");
		wlr_scene_node_destroy(&root->scene->tree.node);
		wlkit_node_destroy(root->node);
		free(root);
		return NULL;
	}

	bool failed = false;
	root->staging = wlkit_alloc_scene_tree(&root->scene->tree, &failed);
	root->layer_tree = wlkit_alloc_scene_tree(&root->scene->tree, &failed);

	if (failed) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit root scene trees");
		wlr_scene_node_destroy(&root->scene->tree.node);
		wlkit_node_destroy(root->node);
		free(root);
		return NULL;
	}

	wlr_scene_node_set_enabled(&root->staging->node, false);

	root->x = 0;
	root->y = 0;
	root->width = 0;
	root->height = 0;

	root->cursor = wlkit_cursor_create(root, NULL, 24);

	wl_list_init(&root->layers);
	wl_list_init(&root->outputs);

	wl_signal_init(&root->events.new_node);

	return root;
}

void wlkit_root_destroy(struct wlkit_root * root) {
	if (!root) {
		return;
	}

	wlr_scene_node_destroy(&root->scene->tree.node);
	wlkit_node_destroy(root->node);
	free(root);
}
