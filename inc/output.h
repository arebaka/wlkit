#ifndef WLKIT_OUTPUT_H
#define WLKIT_OUTPUT_H

#include "common.h"

#include <wayland-server-core.h>
#include <wlr/types/wlr_output.h>

struct wlkit_output {
	struct wlkit_server * server;
	struct wlr_output * wlr_output;
	struct wlr_scene_output * scene_output;

	struct wl_event_source * repaint_timer;

	struct timespec last_frame;
	struct wl_list link;

	struct {
		struct wl_listener layout_destroy;
		struct wl_listener frame;
		struct wl_listener destroy;
		struct wl_listener present;
		struct wl_listener request_state;
	} listeners;

	void * user_data;
};

struct wlkit_output * wlkit_output_create(
	struct wlkit_server * server,
	struct wlr_output * wlr_output,
	struct wlr_scene_output * scene_output
);

#endif // WLKIT_OUTPUT_H
