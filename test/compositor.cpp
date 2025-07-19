#include <iostream>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <unistd.h>

#include <wlkit/wlkit.hpp>

extern "C" {
#include <wlr/types/wlr_seat.h>
#include <wlr/render/swapchain.h>
#include <wlr/render/wlr_texture.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/wlr_texture.h>
#include <wlr/types/wlr_output.h>

#include <xkbcommon/xkbcommon.h>
}

const char * RESET   = "\033[0m";
const char * BOLD    = "\033[1m";
const char * GREEN   = "\033[32m";
const char * YELLOW  = "\033[33m";
const char * BLUE    = "\033[34m";
const char * MAGENTA = "\033[35m";
const char * CYAN    = "\033[36m";
const char * RED     = "\033[31m";

static wlkit::Geo cursor_x = 0, cursor_y = 0;
static bool cursor_state = 0;
static wlkit::Window * moving_window = nullptr;

std::string get_current_time() {
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

void print_separator() {
	std::cout << std::string(60, '=') << std::endl;
}

void print_workspace_info(wlkit::Server * server) {
	std::cout << BOLD << BLUE << "🏢 WORKSPACES INFO" << RESET << std::endl;
	print_separator();

	int workspace_count = 0;
	for (auto* ws : server->workspaces()) {
		workspace_count++;

		std::cout << CYAN << "Workspace #" << ws->id() << RESET;
		if (ws->name() && strlen(ws->name()) > 0) {
			std::cout << " (" << YELLOW << ws->name() << RESET << ")";
		}
		std::cout << std::endl;

		// Показываем активный воркспейс
		bool is_current = false;
		for (auto* output : server->outputs()) {
			if (output->current_workspace() == ws) {
				is_current = true;
				break;
			}
		}

		if (is_current) {
			std::cout << "  " << GREEN << "✓ ACTIVE" << RESET << std::endl;
		}

		// Подсчитываем окна в воркспейсе
		int window_count = 0;
		for (auto* win : server->windows()) {
			if (win->workspace() == ws) {
				window_count++;
			}
		}

		std::cout << "  Windows: " << window_count << std::endl;

		if (ws->layout()) {
			std::cout << "  Layout: " << ws->layout()->name() << std::endl;
		}

		if (ws->focused_window() && ws->focused_window()->ready()) {
			std::cout << "  Focused: " << MAGENTA << ws->focused_window()->title() << RESET << std::endl;
		}

		std::cout << std::endl;
	}

	std::cout << BOLD << "Total workspaces: " << workspace_count << RESET << std::endl;
	std::cout << std::endl;
}

void print_window_info(wlkit::Server * server) {
	std::cout << BOLD << BLUE << "🪟 WINDOWS INFO" << RESET << std::endl;
	print_separator();

	int window_count = 0;
	for (auto* win : server->windows()) {
		window_count++;

		std::cout << CYAN << "Window #" << window_count << RESET;
		if (win->title() && strlen(win->title()) > 0) {
			std::cout << " - " << YELLOW << win->title() << RESET;
		}
		std::cout << std::endl;

		// Информация о приложении
		if (win->app_id() && strlen(win->app_id()) > 0) {
			std::cout << "  App ID: " << win->app_id() << std::endl;
		}

		// Позиция и размеры
		std::cout << "  Position: (" << win->x() << ", " << win->y() << ")" << std::endl;
		std::cout << "  Size: " << win->width() << "x" << win->height() << std::endl;

		// Воркспейс
		if (win->workspace()) {
			std::cout << "  Workspace: #" << win->workspace()->id();
			if (win->workspace()->name() && strlen(win->workspace()->name()) > 0) {
				std::cout << " (" << win->workspace()->name() << ")";
			}
			std::cout << std::endl;
		}

		// Статус окна
		std::cout << "  Status: ";
		if (win->mapped()) {
			std::cout << GREEN << "Mapped" << RESET;
		} else {
			std::cout << RED << "Unmapped" << RESET;
		}

		// Проверяем, является ли окно сфокусированным
		bool is_focused = false;
		for (auto* ws : server->workspaces()) {
			if (ws->focused_window() == win) {
				is_focused = true;
				break;
			}
		}

		if (is_focused) {
			std::cout << " | " << MAGENTA << "Focused" << RESET;
		}

		std::cout << std::endl;
		std::cout << std::endl;
	}

	std::cout << BOLD << "Total windows: " << window_count << RESET << std::endl;
	std::cout << std::endl;
}

void print_output_info(wlkit::Server * server) {
	std::cout << BOLD << BLUE << "🖥️ OUTPUTS INFO" << RESET << std::endl;
	print_separator();

	int output_count = 0;
	for (auto* output : server->outputs()) {
		output_count++;

		std::cout << CYAN << "Output #" << output_count << RESET << std::endl;
		std::cout << "  Resolution: " << output->width() << "x" << output->height() << std::endl;

		if (output->current_workspace()) {
			std::cout << "  Current workspace: #" << output->current_workspace()->id();
			if (output->current_workspace()->name() && strlen(output->current_workspace()->name()) > 0) {
				std::cout << " (" << output->current_workspace()->name() << ")";
			}
			std::cout << std::endl;
		}

		// Показываем воркспейсы доступные на этом выходе
		std::cout << "  Available workspaces: ";
		bool first = true;
		for (auto* ws : output->workspaces()) {
			if (!first) std::cout << ", ";
			std::cout << "#" << ws->id();
			if (ws->name() && strlen(ws->name()) > 0) {
				std::cout << "(" << ws->name() << ")";
			}
			first = false;
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

	std::cout << BOLD << "Total outputs: " << output_count << RESET << std::endl;
	std::cout << std::endl;
}

void print_input_info(wlkit::Server * server) {
	std::cout << BOLD << BLUE << "⌨️ INPUT DEVICES INFO" << RESET << std::endl;
	print_separator();

	int input_count = 0;
	for (auto* input : server->inputs()) {
		input_count++;

		std::cout << CYAN << "Input #" << input_count << RESET << " - ";

		if (input->is_keyboard()) {
			std::cout << "Keyboard" << std::endl;
		} else if (input->is_pointer()) {
			std::cout << "Pointer" << std::endl;
		} else {
			std::cout << "Other" << std::endl;
		}
	}

	std::cout << BOLD << "Total input devices: " << input_count << RESET << std::endl;
	std::cout << std::endl;
}

void print_full_status(wlkit::Server * server) {
	std::cout << BOLD << GREEN << "🔧 WLKIT COMPOSITOR STATUS" << RESET << std::endl;
	std::cout << "Time: " << get_current_time() << std::endl;
	std::cout << std::endl;

	print_output_info(server);
	print_workspace_info(server);
	print_window_info(server);
	print_input_info(server);

	print_separator();
	std::cout << BOLD << "Status update complete!" << RESET << std::endl;
}

void setup_portal_env(const wlkit::Server * server) {
	setenv("XDG_CURRENT_DESKTOP", "wlkit", true);
	setenv("XDG_SESSION_TYPE", "wayland", true);
	setenv("XDG_SESSION_DESKTOP", "wlkit", true);
}

void ai_test_draw_frame(wlkit::Output * output, struct wlr_output * wlr_output, wlkit::Render * render) {
	struct wlr_render_pass * pass = render->pass();

	// 5. Добавим заливку (фон)
	struct wlr_render_rect_options rect_opts = {
		.box = {
			.x = 0, .y = 0,
			.width = output->width(),
			.height = output->height(),
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
			.width = output->width(),
			.height = output->height(),
		},
		.color = { bg_r, bg_g, bg_b, 1.0f },
	};
	wlr_render_pass_add_rect(pass, &bg_opts);

	// 4. Анимированные круги (имитация)
	int center_x = output->width() / 2;
	int center_y = output->height() / 2;

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
	int wave_y = output->height() - wave_height - 50;

	for (int x = 0; x < output->width(); x += 4) {
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
		int corner_x = (corner % 2) * (output->width() - 100) + 50;
		int corner_y = (corner / 2) * (output->height() - 100) + 50;

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

void ai_test_draw_status(wlkit::Output * output, struct wlr_output * wlr_output, wlkit::Render * render) {
	// Получаем render_pass
	struct wlr_render_pass *pass = render->pass();

	// ––––––––––––––––––––––––––––––––––––––––––
	// 2) Панель воркспейсов
	// ––––––––––––––––––––––––––––––––––––––––––
	auto server = output->server();
	auto workspaces = server->workspaces();
	size_t ws_count = workspaces.size();
	const int tab_h = 30;
	int tab_w = output->width() / (ws_count ? ws_count : 1);

	size_t idx = 0;
	for (auto ws : workspaces) {
		bool current = ws == output->current_workspace();
		struct wlr_render_rect_options ws_opts = {
			.box = {
				.x = int(idx * tab_w) + 2,
				.y = 2,
				.width  = tab_w - 4,
				.height = tab_h - 4,
			},
			.color = { current ? 0.2f : 0.3f, current ? 0.6f : 0.3f, current ? 0.9f : 0.3f, 1.0f }
		};
		wlr_render_pass_add_rect(pass, &ws_opts);
		idx++;
	}

	// ––––––––––––––––––––––––––––––––––––––––––
	// 3) Окна на текущем воркспейсе
	// ––––––––––––––––––––––––––––––––––––––––––
	for (auto win : output->current_workspace()->windows()) {
		if (!win->xdg_surface()) {
			// получаем геометрию окна
			int x = win->x();
			int y = win->y() + tab_h;  // сдвиг вниз под панель
			int w = win->width();
			int h = win->height();

			bool focused = win == win->workspace()->focused_window();
			struct wlr_render_rect_options w_opts = {
				.box = { .x = x, .y = y, .width = w, .height = h },
				.color = { focused ? 0.8f : 0.5f, focused ? 0.8f : 0.5f, focused ? 0.2f : 0.5f, focused ? 0.8f : 0.5f }
			};
			wlr_render_pass_add_rect(pass, &w_opts);
			continue;
		}

		if (!win->ready() || !win->dirty()) {
			continue;
		}

		typedef struct {
			wlkit::Window * window;
			wlkit::Output * output;
			wlr_render_pass * pass;
		} Context;

		auto context = new Context{
			.window = win,
			.output = output,
			.pass = pass,
		};

		auto xdg_surface = win->xdg_surface();
		wlr_xdg_surface_for_each_surface(xdg_surface, [](auto surface, auto sx, auto sy, auto data) {
			auto context = static_cast<Context*>(data);
			auto win = context->window;
			auto output = context->output;
			auto pass = context->pass;

			auto texture = wlr_surface_get_texture(surface);
			if (!texture) {
				return;
			}

			uint32_t tex_w = surface->current.width;
			uint32_t tex_h = surface->current.height;

			struct wlr_box dst = {
				.x = win->x(),
				.y = win->y(),
				.width = tex_w,
				.height = tex_h,
			};

			struct wlr_fbox src = {
				.x = 0,
				.y = 0,
				.width = tex_w,
				.height = tex_h,
			};

			struct wlr_render_texture_options opts = {
				.texture = texture,
				.src_box = src,
				.dst_box = dst,
				.alpha = NULL,
				.transform = output->get_transform(),
			};

			wlr_render_pass_add_texture(pass, &opts);
		}, context);

		win->drawn();
	}
}

void draw_cursor(wlkit::Output * output, struct wlr_output * wlr_output, wlkit::Render * render) {
	struct wlr_render_pass * pass = render->pass();

	const float color = cursor_state ? 1.0f : 0.0f;

	struct wlr_render_rect_options rect_opts = {
		.box = { cursor_x, cursor_y, 20, 20 },
		.color = { color, color, color, 1.0f }
	};
	wlr_render_pass_add_rect(pass, &rect_opts);
}

void setup_output(wlkit::Output * output, struct wlr_output * wlr_output, wlkit::Server * server) {
	auto state = wlkit::OutputStateBuilder{}
		.enabled(true)
		.scale(1.0f)
		.transform(WL_OUTPUT_TRANSFORM_NORMAL)
		.adaptive_sync_enabled(true)
		.render_format(wlr_output->render_format)
		.build();

	output->
		setup_state(state.get())
		.setup_preferred_mode()
		.commit_state();
}

void setup_keyboard(wlkit::Keyboard * keyboard) {
	keyboard->
		set_rules("base")
		.set_model("pc105")
		.set_layout("us,ru")
		.set_variant("")
		.set_options("ctrl:nocaps,grp:alt_shift_toggle,grp_led:caps,lv3:ralt_switch,altwin:meta_alt,compose:rctrl,numpad:pc,nbsp:level3,terminate:ctrl_alt_bksp")
		.compile_keymap();

	keyboard->
		on_key_pressed([](auto keyboard, auto keycode) {
			std::cout << "Key pressed: code=" << keycode << std::endl;
		})
		.on_key_released([](auto keyboard, auto keycode) {
			auto sym = xkb_state_key_get_one_sym(keyboard->wlr_keyboard()->xkb_state, keycode + 8);
			char name[64];
			xkb_keysym_get_name(sym, name, sizeof(name));
			std::cout << "Key released: code=" << keycode << " name=" << name  << std::endl;
		})
		.on_mod([](auto keyboard, auto mods) {
			std::cout << "Modifiers changed:"
				<< " depressed=" << mods->depressed
				<< " latched="   << mods->latched
				<< " locked="    << mods->locked
				<< " group="     << std::dec << mods->group
				<< std::endl;
		})
		.on_repeat([](auto keyboard, auto rate, auto delay) {
			std::cout << "Repeat rate=" << rate << "Hz, delay=" << delay << "ms\n" << std::endl;
		})
		.on_key_pressed([](auto keyboard, auto keycode) {
			auto server = keyboard->server();
			auto sym = xkb_state_key_get_one_sym(keyboard->wlr_keyboard()->xkb_state, keycode + 8);
			if (sym == XKB_KEY_Escape) {
				server->stop();
			} else if (sym > XKB_KEY_0 && sym < XKB_KEY_9) {
				auto output = *server->outputs().begin();
				auto workspace = server->get_workspace_by_id(sym - XKB_KEY_0);
				output->switch_to_workspace(workspace);
				if (moving_window) {
					moving_window->set_workspace(workspace);
				}
			}
		})
		.on_key_pressed([](auto keyboard, auto keycode) {
			auto server = keyboard->server();
			auto sym = xkb_state_key_get_one_sym(keyboard->wlr_keyboard()->xkb_state, keycode + 8);
			if (sym == XKB_KEY_F4) {
				print_window_info(server);
			}
		});
}

void setup_pointer(wlkit::Pointer * pointer) {
	pointer->
		on_motion([](auto pointer, auto dx, auto dy, auto unaccel_dx, auto unaccel_dy) {
			cursor_x += dx;
			cursor_y += dy;
			if (moving_window) {
				moving_window->move(moving_window->x() + dx, moving_window->y() + dy);
			}

			auto output = pointer->server()->outputs().front();
			if (cursor_x < 0) {
				cursor_x = output->width();
			}
			if (cursor_y < 0) {
				cursor_y = output->height();
			}
			if (cursor_x > output->width()) {
				cursor_x = 0;
			}
			if (cursor_y > output->height()) {
				cursor_y = 0;
			}
		})
		.on_button([](auto pointer, auto button, auto state) {
			if (button == 272) {
				cursor_state = state;
				if (state == 1) {
					auto output = *pointer->server()->outputs().begin();
					auto window = output->window_at(cursor_x, cursor_y);
					if (window) {
						output->current_workspace()->focus_window(window);
						moving_window = window;
					}
				} else {
					moving_window = nullptr;
				}
			}
		});
}

void setup_input(wlkit::Input * input, struct wlr_input_device * device, wlkit::Server * server) {
	if (input->is_keyboard()) {
		return setup_keyboard(input->as_keyboard());
	}
	if (input->is_pointer()) {
		return setup_pointer(input->as_pointer());
	}
}

void create_defaults(wlkit::Output * output, struct wlr_output * wlr_output, wlkit::Server * server) {
	auto layout = new wlkit::Layout("floating", nullptr);
	auto workspace1 = new wlkit::Workspace(server, layout, 1, "main", nullptr);
	auto workspace2 = new wlkit::Workspace(server, layout, 2, "work", nullptr);
	auto workspace3 = new wlkit::Workspace(server, layout, 3, "media", nullptr);

	auto window1 = new wlkit::Window(server, workspace1,
		nullptr, nullptr, "Terminal", "kitty", nullptr);
	auto window2 = new wlkit::Window(server, workspace1,
		nullptr, nullptr, "Web Browser", "firefox", nullptr);
	auto window3 = new wlkit::Window(server, workspace2,
		nullptr, nullptr, "Text Editor", "code", nullptr);

	window1->move(10, 10).resize(400, 300).map();
	window2->move(200, 100).resize(500, 400).map();
	window3->move(0, 0).resize(800, 600).map();

	workspace1->focus_window(window1);
	workspace1->focus_window(window2);
	workspace2->focus_window(window3);

	output->switch_to_workspace(workspace1);
}

static void launch_program(const char * name, std::vector<const char*> args) {
	if (fork() == 0) {
		args.push_back(nullptr);
		execvp(name, const_cast<char* const*>(args.data()));
		perror("execvp failed");
		_exit(1);
	}
}

int main() {
	wlr_log_init(WLR_INFO, NULL);
	auto seat = wlkit::Seat("seat0", nullptr);
	auto server = wlkit::Server(&seat, setup_portal_env);

	server
		.on_start(setup_portal_env)
		.on_start([](auto server) {
			std::cout << GREEN << "✓ Server started successfully!" << RESET << std::endl;
			print_full_status(server);
			wl_event_loop_add_idle(server->event_loop(), [](void * data) {
				launch_program("weston-simple-shm", {});
			}, nullptr);
		})
		.on_stop([](auto server) {
			std::cout
				<< "outputs: " << server->outputs().size() << std::endl
				<< "inputs: " << server->inputs().size() << std::endl
				<< "workspaces: " << server->workspaces().size() << std::endl
				<< "windows: " << server->windows().size() << std::endl
				<< "Escape!" << std::endl;
		})
		.on_new_output([](auto output, auto wlr_output, auto server) {
			output->server()->prefer_output(output);
			// output->on_frame(dummy_draw_frame);
			output->on_frame(ai_test_draw_frame);
			output->on_frame(ai_test_draw_status);
			output->on_frame(draw_cursor);
		})
		.on_new_input(setup_input)
		.on_new_output(setup_output)
		.on_new_output(create_defaults)
		.on_new_xdg_shell_toplevel([](auto window, auto xdg_surface, auto output) {
			window->move(0 ,0).resize(500, 500);
		})
		.start();
}
