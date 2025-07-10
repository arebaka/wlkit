#include <stdio.h>
#include <wlkit/server.h>
#include <wlkit/wlkit.h>

#include <wlr/types/wlr_seat.h>

#include <wlr/render/swapchain.h>
#include <cairo/cairo.h>
#include <wlr/render/wlr_texture.h>

void setup_portal_env(struct wlkit_server * server) {
	(void)server;  // unused for now
	setenv("XDG_CURRENT_DESKTOP", "wlkit", 1);
	setenv("XDG_SESSION_TYPE", "wayland", 1);
	setenv("XDG_SESSION_DESKTOP", "wlkit", 1);
}

void on_destroy(struct wlkit_server * server) {
	char info[256];
	snprintf(info, sizeof(info),
		"Exit; Out:%u In:%u WS:%u Win:%u Seat:%s\n",
		wl_list_length(&server->outputs),
		wl_list_length(&server->inputs),
		wl_list_length(&server->workspaces),
		wl_list_length(&server->windows),
		server->seat->name
	);
	printf(info);
}

void create_default_workspace(struct wlkit_server * server) {
	server->current_workspace = wlkit_workspace_create(server, &wlkit_layout_floating, 1, "default");
}

void setup_ouput_mode(struct wl_listener * listener, void * data, struct wlkit_server * server) {
	struct wlr_output * wlr_output = data;

	struct wlr_output_state state;
	wlr_output_state_init(&state);
	wlr_output_state_set_enabled(&state, true);
	wlr_output_state_set_scale(&state, 1.0f);
	wlr_output_state_set_transform(&state, WL_OUTPUT_TRANSFORM_NORMAL);

	struct wlr_output_mode * mode = wlr_output_preferred_mode(wlr_output);
	if (mode) {
		wlr_output_state_set_mode(&state, mode);
	}

	wlr_output_commit_state(wlr_output, &state);
	wlr_output_state_finish(&state);
}

void ai_test_draw_frame(struct wl_listener * listener, void * data, struct wlkit_server * server) {
	struct wlkit_output * output = wl_container_of(listener, output, listeners.frame);
	struct wlr_output * wlr_output = output->wlr_output;
	struct wlr_allocator * allocator = server->allocator;
	struct wlr_renderer * renderer = server->renderer;

	// 1. Подготовим состояние вывода
	struct wlr_output_state state;
	wlr_output_state_init(&state);

	// 2. Настроим swapchain
	if (!wlr_output_configure_primary_swapchain(wlr_output, NULL, &wlr_output->swapchain)) {
		wlr_log(WLR_ERROR, "Failed to configure swapchain");
		goto cleanup;
	}

	// 3. Получим буфер из swapchain
	struct wlr_buffer *buffer = wlr_swapchain_acquire(wlr_output->swapchain);
	if (!buffer) {
		wlr_log(WLR_ERROR, "Failed to acquire buffer from swapchain");
		goto cleanup;
	}

	// 4. Создаём render_pass
	struct wlr_buffer_pass_options pass_opts = {
		// Можно добавить .damaged, если есть damage tracking
	};
	struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, &pass_opts);
	if (!pass) {
		wlr_log(WLR_ERROR, "Failed to begin render pass");
		wlr_buffer_drop(buffer);
		goto cleanup;
	}

	// 5. Добавим заливку (фон)
	struct wlr_render_rect_options rect_opts = {
		.box = {
			.x = 0, .y = 0,
			.width = wlr_output->width,
			.height = wlr_output->height,
		},
		.color = { 0.48828125f, 0.5f, 0.71875f, 1.0f },
	};
	wlr_render_pass_add_rect(pass, &rect_opts);

	// 7) Прогресс‑бар секунд внизу
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	float frac = tm->tm_sec / 60.0f;
	int bar_width = (int)(wlr_output->width * frac);
	struct wlr_render_rect_options secbar = {
		.box = { .x = 0,
				 .y = wlr_output->height - 200,
				 .width = bar_width,
				 .height = 20 },
		.color = { 0.2f, 0.6f, 1.0f, 0.8f },
	};
	wlr_render_pass_add_rect(pass, &secbar);

	// 6. Нарисуем курсоры поверх
	wlr_output_add_software_cursors_to_render_pass(wlr_output, pass, NULL);

	// 7. Завершаем рендер
	if (!wlr_render_pass_submit(pass)) {
		wlr_log(WLR_ERROR, "Failed to submit render pass");
		wlr_buffer_drop(buffer);
		goto cleanup;
	}

	// 8. Присваиваем буфер для вывода
	wlr_output_state_set_buffer(&state, buffer);

	// 9. Коммитим
	if (!wlr_output_commit_state(wlr_output, &state)) {
		wlr_log(WLR_ERROR, "Failed to commit output");
	}

	wlr_buffer_drop(buffer);

cleanup:
	wlr_output_state_finish(&state);
}

int main() {
	struct wl_display * display = wl_display_create();
	struct wlr_seat * seat = wlr_seat_create(display, "seat0");

	struct wlkit_server * server = wlkit_create(display, seat, setup_portal_env);

	wlkit_on_destroy(server, on_destroy);
	wlkit_on_start(server, create_default_workspace);
	wlkit_on_new_output(server, setup_ouput_mode);
	wlkit_on_output_frame(server, ai_test_draw_frame);

	wlkit_start(server);
	wlkit_run(server);

	wlkit_stop(server);
	wlkit_destroy(server);
}
