#include <iostream>
#include <cstdlib>

#include <wayland-server-protocol.h>
#include <wlkit/wlkit.hpp>

extern "C" {
#include <wlr/types/wlr_seat.h>
#include <wlr/render/swapchain.h>
#include <wlr/render/wlr_texture.h>
}


void setup_portal_env(const wlkit::Server & server) {
	(void)server;  // unused for now
	setenv("XDG_CURRENT_DESKTOP", "wlkit", 1);
	setenv("XDG_SESSION_TYPE", "wayland", 1);
	setenv("XDG_SESSION_DESKTOP", "wlkit", 1);
}

void on_destroy(const wlkit::Server & server) {
	char info[256];
	// snprintf(info, sizeof(info),
	// 	"Exit; Out:%u In:%u WS:%u Win:%u Seat:%s\n",
	// 	&server._outputs.size(),
	// 	&server._inputs.size(),
	// 	&server._workspaces.size(),
	// 	&server._windows.size(),
	// 	server.seat()->name
	// );
	std::cout << info;
}

// void create_default_workspace(struct wlkit::Server & server) {
// 	server.set_current_workspace = new wlkit::Workspace(server, &wlkit_layout_floating, 1, "default");
// }

void dummy_draw_frame(struct wl_listener * listener, void * data, wlkit::Object object) {
	auto render = object.render;
	struct wlr_render_rect_options rect_opts = {
		.box = { 0, 0, 100, 100 },
		.color = { 1.0f, 0.0f, 0.0f, 1.0f },
	};
	wlr_render_pass_add_rect(render->pass(), &rect_opts);
}

void ai_test_draw_frame(struct wl_listener * listener, void * data, wlkit::Object object) {
	auto render = object.render;
	auto output = render->output();
	auto wlr_output = output->wlr_output();
	struct wlr_render_pass * pass = render->pass();

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

	// Получаем время для анимации
	time_t now = time(nullptr);
	struct tm *tm = localtime(&now);
	if (!tm) {
		return;
	};

	// Общее время в секундах с начала минуты для плавной анимации
	float time_sec = tm->tm_sec + (float)(clock() % CLOCKS_PER_SEC) / CLOCKS_PER_SEC;

	// 3. Градиентный фон (меняется со временем)
	float bg_r = 0.1f + 0.1f * sinf(time_sec * 0.1f);
	float bg_g = 0.15f + 0.15f * sinf(time_sec * 0.15f);
	float bg_b = 0.3f + 0.2f * sinf(time_sec * 0.2f);

	struct wlr_render_rect_options bg_opts = {
		.box = {
			.x = 0, .y = 0,
			.width = wlr_output->width,
			.height = wlr_output->height,
		},
		.color = { bg_r, bg_g, bg_b, 1.0f },
	};
	wlr_render_pass_add_rect(pass, &bg_opts);

	// 4. Анимированные круги (имитация)
	int center_x = wlr_output->width / 2;
	int center_y = wlr_output->height / 2;

	for (int i = 0; i < 5; i++) {
		float angle = time_sec * 0.5f + i * 1.26f; // 1.26 ≈ 2π/5
		float radius = 100.0f + 50.0f * sinf(time_sec * 0.3f + i);

		int x = center_x + (int)(radius * cosf(angle));
		int y = center_y + (int)(radius * sinf(angle));

		// Размер "круга" (квадрата)
		int size = 30 + (int)(15 * sinf(time_sec * 0.8f + i * 0.5f));

		float hue = (i * 0.2f + time_sec * 0.1f);
		while (hue > 1.0f) hue -= 1.0f;

		// Простое HSV -> RGB преобразование
		float c = 0.8f; // насыщенность
		float v = 0.9f; // яркость
		float h = hue * 6.0f;
		float x_color = c * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));

		float r, g, b;
		if (h < 1.0f) { r = c; g = x_color; b = 0; }
		else if (h < 2.0f) { r = x_color; g = c; b = 0; }
		else if (h < 3.0f) { r = 0; g = c; b = x_color; }
		else if (h < 4.0f) { r = 0; g = x_color; b = c; }
		else if (h < 5.0f) { r = x_color; g = 0; b = c; }
		else { r = c; g = 0; b = x_color; }

		r = (r + (v - c)) * 0.8f;
		g = (g + (v - c)) * 0.8f;
		b = (b + (v - c)) * 0.8f;

		struct wlr_render_rect_options circle_opts = {
			.box = {
				.x = x - size/2,
				.y = y - size/2,
				.width = size,
				.height = size,
			},
			.color = { r, g, b, 0.8f },
		};
		wlr_render_pass_add_rect(pass, &circle_opts);
	}

	// 5. Волновой эффект внизу
	int wave_height = 60;
	int wave_y = wlr_output->height - wave_height - 50;

	for (int x = 0; x < wlr_output->width; x += 4) {
		float wave_offset = sinf((x * 0.01f) + (time_sec * 2.0f)) * 20.0f;
		int bar_height = wave_height + (int)wave_offset;

		float wave_intensity = (sinf(x * 0.005f + time_sec) + 1.0f) * 0.5f;

		struct wlr_render_rect_options wave_opts = {
			.box = {
				.x = x,
				.y = wave_y - bar_height/2,
				.width = 4,
				.height = bar_height,
			},
			.color = { 0.2f + wave_intensity * 0.6f, 0.6f, 1.0f, 0.7f },
		};
		wlr_render_pass_add_rect(pass, &wave_opts);
	}

	// 6. Цифровые часы в центре
	int clock_bg_width = 200;
	int clock_bg_height = 80;
	struct wlr_render_rect_options clock_bg_opts = {
		.box = {
			.x = center_x - clock_bg_width/2,
			.y = center_y - clock_bg_height/2,
			.width = clock_bg_width,
			.height = clock_bg_height,
		},
		.color = { 0.0f, 0.0f, 0.0f, 0.7f },
	};
	wlr_render_pass_add_rect(pass, &clock_bg_opts);

	// 7. Простая визуализация времени через полоски
	// Часы (0-23)
	int hour_width = (tm->tm_hour * clock_bg_width) / 24;
	if (hour_width > 0) {
		struct wlr_render_rect_options hour_opts = {
			.box = {
				.x = center_x - clock_bg_width/2,
				.y = center_y - 30,
				.width = hour_width,
				.height = 15,
			},
			.color = { 1.0f, 0.2f, 0.2f, 0.9f },
		};
		wlr_render_pass_add_rect(pass, &hour_opts);
	}

	// Минуты (0-59)
	int min_width = (tm->tm_min * clock_bg_width) / 60;
	if (min_width > 0) {
		struct wlr_render_rect_options min_opts = {
			.box = {
				.x = center_x - clock_bg_width/2,
				.y = center_y - 10,
				.width = min_width,
				.height = 15,
			},
			.color = { 0.2f, 1.0f, 0.2f, 0.9f },
		};
		wlr_render_pass_add_rect(pass, &min_opts);
	}

	// Секунды (0-59)
	int sec_width = (tm->tm_sec * clock_bg_width) / 60;
	if (sec_width > 0) {
		struct wlr_render_rect_options sec_opts = {
			.box = {
				.x = center_x - clock_bg_width/2,
				.y = center_y + 10,
				.width = sec_width,
				.height = 15,
			},
			.color = { 0.2f, 0.2f, 1.0f, 0.9f },
		};
		wlr_render_pass_add_rect(pass, &sec_opts);
	}

	// 8. Частицы в углах
	for (int corner = 0; corner < 4; corner++) {
		int corner_x = (corner % 2) * (wlr_output->width - 100) + 50;
		int corner_y = (corner / 2) * (wlr_output->height - 100) + 50;

		for (int p = 0; p < 3; p++) {
			float particle_angle = time_sec * 1.5f + corner * 1.57f + p * 2.1f;
			float particle_dist = 30.0f + 10.0f * sinf(time_sec * 2.0f + p);

			int px = corner_x + (int)(particle_dist * cosf(particle_angle));
			int py = corner_y + (int)(particle_dist * sinf(particle_angle));

			struct wlr_render_rect_options particle_opts = {
				.box = {
					.x = px - 5,
					.y = py - 5,
					.width = 10,
					.height = 10,
				},
				.color = { 1.0f, 1.0f, 1.0f, 0.6f },
			};
			wlr_render_pass_add_rect(pass, &particle_opts);
		}
	}
}

void setup_output(struct wl_listener * listener, void * data, wlkit::Object object) {
	auto wlr_output = static_cast<struct wlr_output*>(data);
	auto output = object.output;

	auto builder = new wlkit::OutputStateBuilder();
	auto state = builder->
		enabled(true)
		.scale(1.0f)
		.transform(WL_OUTPUT_TRANSFORM_NORMAL)
		.adaptive_sync_enabled(true)
		.render_format(wlr_output->render_format)
		.build();

	output->
		setup_state(state)
		.setup_preferred_mode()
		.commit_state();
}

void setup_draw(struct wl_listener * listener, void * data, wlkit::Object object) {
	// object.output->on_frame(dummy_draw_frame);
	object.output->on_frame(ai_test_draw_frame);
}

int main() {
	struct wl_display * display = wl_display_create();
	struct wlr_seat * seat = wlr_seat_create(display, "seat0");

	auto server = new wlkit::Server(*display, *seat, setup_portal_env);
	server->
		on_destroy(on_destroy)
		.on_new_output(setup_draw)
		.on_new_output(setup_output)
		.start()
		.stop();
}
