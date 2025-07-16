# wlkit

> A C++ framework for building Wayland compositors and window managers using wlroots.

---

## 🔧 Installation

```bash
git clone https://github.com/arebaka/wlkit.git
cd wlkit
make && sudo make install
```

## 📘 Quick Start

```cpp
#include <wlkit/wlkit.hpp>

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

int main() {
	auto seat = wlkit::Seat("seat0", nullptr);
	auto server = wlkit::Server(&seat, setup_portal_env);

	server
		.on_start([](auto server) {
			auto layout = new wlkit::Layout("tiling", nullptr);
			auto workspace = new wlkit::Workspace(server, layout, 1, "default", nullptr);
			auto window1 = new wlkit::Window(server, workspace,
				/* xdg surface */ nullptr, /* xwayland surface */ nullptr,
				"test window 1", "app name", nullptr);
			auto window2 = new wlkit::Window(server, workspace,
				nullptr, nullptr, "test window 2", "app name", nullptr);
		})
		// subscribe to rendering
		.on_new_output(setup_output)
		.on_new_output([](auto output, auto wlr_output, auto server) {
			output->on_frame(draw_windows);
			output->on_frame(draw_cursor);
		})
		// handle input
		.on_new_input([](wlkit::Input * input, struct wlr_input_device * device, wlkit::Server * server) {
			if (input->is_keyboard()) {
				return setup_keyboard(input->as_keyboard());
			}
			if (input->is_pointer()) {
				return setup_pointer(input->as_pointer());
			}
		})
		.on_destroy(on_destroy)  // cleanup on exit
		.start();

	return 0;
}
```

---

## 🧭 Usage Examples

### Draw Workspaces and Windows

```cpp
output->on_frame([](wlkit::Output * output, struct wlr_output *, wlkit::Render * render){
	auto pass = render->pass();
	auto server = output->server();

	// Background
	struct wlr_render_rect_options bg = {
		.box = { 0, 0, output->width(), output->height() },
		.color = { 0.1f, 0.1f, 0.1f, 1.0f }
	};
	wlr_render_pass_add_rect(pass, &bg);

	// Workspaces
	int idx = 0;
	for (auto * ws : server->workspaces()) {
		bool current = ws == output->current_workspace();
		float ws_color[4] = {
			current ? 0.0f : 0.4f,
			current ? 0.7f : 0.4f,
			current ? 0.0f : 0.4f,
			0.0f
		};
		struct wlr_render_rect_options ws_rect = {
			.box = { idx * 100, 0, 80, 20 },
			.color = { ws_color[0], ws_color[1], ws_color[2], ws_color[3] }
		};
		wlr_render_pass_add_rect(pass, &ws_rect);
		idx++;
	}

	// Windows
	for (auto * win : server->windows()) {
		if (win->workspace() != output->current_workspace()) continue;
		bool focused = (win == output->current_workspace()->focused_window());
		float win_color[4] = {
			focused ? 1.0f : 0.6f,
			focused ? 1.0f : 0.6f,
			focused ? 0.2f : 0.6f,
			1.0f
		};
		struct wlr_render_rect_options win_rect = {
			.box = { win->x(), win->y(), win->width(), win->height() },
			.color = { win_color[0], win_color[1], win_color[2], win_color[3] }
		};
		wlr_render_pass_add_rect(pass, &win_rect);
	}
});
```

### Switch Window with Mod+Tab

```cpp
keyboard->on_modifiers([=](wl_listener*, wlkit::Keyboard * kb, struct wlr_keyboard_modifiers * mods, struct wlr_keyboard*){
	// add logic for modifier changes here
});

keyboard->on_key_pressed([=](auto keyboard, auto keycode){
	auto kbd = keyboard->wlr_keyboard();
	xkb_keysym_t sym = xkb_state_key_get_one_sym(kbd->xkb_state, keycode + 8);
	if (sym == XKB_KEY_Tab && (kbd->modifiers.depressed & WLR_MODIFIER_LOGO)) {
		keyboard->server()->focus_next_window();
	}
});
```

### Move Window with Cursor

```cpp
cursor->on_motion([=](auto pointer, auto dx, auto dy, auto unaccel_dx, auto unaccel_dy){
	if (auto win = pointer->server()->windows().front()) {
		win->move(dx, dy);
	}
});
```

### Additionally: Visulisation of cursor and text

Even without Cairo, you can display a cursor video using `wlr_render_pass_d_texture`.
If you have `wlr_texture *cursor_tex` and a rectangle, do:

```cpp
struct wlr_render_texture_options co = {
	.box = { cur_x, cur_y, tex_w, tex_h },
	.alpha = 1.0f,
};
wlr_render_pass_add_texture(pass, cursor_tex, &co);
```

---

## ⚙️ Configuring the workflow

- Create wlkit::Server: specify `on_new_output`, `on_new_intput`.
- In `on_new_output` - subscribe to `on_frame` to draw.
- In `on_new_input` - process the keyboard via `as_keyboard()` and the mouse via `as_pointer()`.
- In `on_frame` – render overflows, windows, cursor and content.

---

## 🧠 API Notes

- `wlkit::OutputStateBuilder` simplifies the creation of a `wlkit::Output::State` for `wlkit::Ouput::setup_state()`.
- `wlkit::Server` gives access to `.outputs()`, `.inputs()`, `.windows()`, `.workspaces()`, etc.
- `wlkit::Output` gives access to `.server()`, `.current_workspace()`, `.workspaces()`, `.width()`, `.height()`, etc.
- `wlkit::Workspace` gives access to `.server()`, `.layout()`, `.id()`, `.name()`, `.focused_window()`, `.window()`, etc.
- `wlkit::Window` gives access to `.server()`, `.workspace()`, `.title()`, `.app_id()`, `.x()`, `.y()`, `.width()`, `.height()`, `.mapped()`, etc.

---

https://github.com/user-attachments/assets/786b9ce1-6e4c-4249-9161-346d3877e97c
