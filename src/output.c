#include "wlkit.h"

struct wlkit_output * wlkit_output_create(
	struct wlkit_server * server,
	struct wlr_output * wlr_output,
	struct wlr_scene_output * scene_output
) {
	struct wlkit_output * output = malloc(sizeof(*output));
	if (!output) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit output");
		return NULL;
	}

	output->server = server;
	output->wlr_output = wlr_output;
	output->scene_output = scene_output;
	return output;
}
