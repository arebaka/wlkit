#include "wlkit.h"

static void handle_lauout_destroy(struct wl_listener * listener, void * data) {
}

static void handle_destroy(struct wl_listener * listener, void * data) {
}

static void handle_frame(struct wl_listener * listener, void * data) {
	struct wlkit_output * output = wl_container_of(listener, output, listeners.frame);
	struct wlr_output * wlr_output = output->wlr_output;
	if (!wlr_output) {
		wlr_log(WLR_ERROR, "Invalid output");
		return;
	}

	struct wlr_buffer_pass_options pass_opts = {};
	struct wlkit_render * render = wlkit_render_create(output, &pass_opts);

	union wlkit_object object = { .render = render };
	struct wlkit_notify_handler * wrapper;
	wl_list_for_each(wrapper, &output->handlers.frame, link) {
		wrapper->handler(listener, data, &object);
	}

	wlkit_render_commit(render);
}

static void handle_present(struct wl_listener * listener, void * data) {
}

static void handle_request_state(struct wl_listener * listener, void * data) {
}

static int handle_repaint_timer(void * data) {
}

struct wlkit_output * wlkit_output_create(
	struct wlkit_server * server,
	struct wlr_output * wlr_output
) {
	if (!server || !wlr_output) {
		return NULL;
	}

	struct wlkit_output * output = malloc(sizeof(*output));
	if (!output) {
		wlr_log(WLR_ERROR, "Unable to allocate wlkit output");
		return NULL;
	}

	output->server = server;
	output->wlr_output = wlr_output;
	output->user_data = NULL;

	output->scene_output = wlr_scene_output_create(server->root->scene, wlr_output);
	if (!output->scene_output) {
		wlr_log(WLR_ERROR, "Failed to create a wlkit scene output");
		free(output);
		return NULL;
	}

	wl_list_init(&output->handlers.create);
	wl_list_init(&output->handlers.destroy);
	wl_list_init(&output->handlers.layout_destroy);
	wl_list_init(&output->handlers.frame);
	wl_list_init(&output->handlers.present);
	wl_list_init(&output->handlers.request_state);
	wl_list_init(&output->handlers.repaint_timer);

	if (!wlr_output_init_render(wlr_output, server->allocator, server->renderer)) {
		wlr_log(WLR_ERROR, "Failed to init wlkit output render");
		wlr_scene_output_destroy(output->scene_output);
		free(output);
		return NULL;
	}

	wl_signal_add(&server->root->output_layout->events.destroy, &output->listeners.layout_destroy);
	output->listeners.layout_destroy.notify = handle_lauout_destroy;
	wl_signal_add(&wlr_output->events.destroy, &output->listeners.destroy);
	output->listeners.destroy.notify = handle_destroy;
	wl_signal_add(&wlr_output->events.frame, &output->listeners.frame);
	output->listeners.frame.notify = handle_frame;
	wl_signal_add(&wlr_output->events.present, &output->listeners.present);
	output->listeners.present.notify = handle_present;
	wl_signal_add(&wlr_output->events.request_state, &output->listeners.request_state);
	output->listeners.request_state.notify = handle_request_state;

	output->repaint_timer = wl_event_loop_add_timer(server->event_loop, handle_repaint_timer, output);

	wlr_output_layout_add_auto(server->root->output_layout, wlr_output);
	wl_list_insert(&server->outputs, &output->link);

	struct wlkit_output_handler * wrapper;
	wl_list_for_each(wrapper, &output->handlers.create, link) {
		wrapper->handler(output);
	}

	return output;
}

void wlkit_output_destroy(struct wlkit_output * output) {
	if (!output) {
		return;
	}

	wlr_scene_output_destroy(output->scene_output);
	free(output);
}

static void assume_handler(struct wl_list * handlers, wlkit_output_handler_t handler) {
	struct wlkit_output_handler * wrapper = malloc(sizeof(*wrapper));
	wrapper->handler = handler;
	wl_list_insert(handlers, &wrapper->link);
}

void wlkit_on_output_create(struct wlkit_output * output, wlkit_output_handler_t handler) {
	assume_handler(&output->handlers.create, handler);
}

void wlkit_on_output_destroy(struct wlkit_output * output, wlkit_output_handler_t handler) {
	assume_handler(&output->handlers.destroy, handler);
}

void wlkit_on_output_frame(struct wlkit_output * output, wlkit_notify_handler_t handler) {
	wlkit_assume_notify_handler(&output->handlers.frame, handler);
}
