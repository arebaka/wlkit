#pragma once

extern "C" {
#include <wlr/types/wlr_keyboard.h>
}

#include "input.hpp"

namespace wlkit {

class Keyboard : public Input {
public:
	using Keycode = uint32_t;
	using KeyHandler = std::function<
		void(struct ::wlr_keyboard_key_event * event, Keyboard * keyboard, struct ::wlr_keyboard * kbd)>;
	using ModHandler = std::function<
		void( struct ::wlr_keyboard_modifiers * mods, Keyboard * keyboard, struct ::wlr_keyboard * kbd)>;
	// using RepeatHandler = std::function<
	// 	void(struct wl_listener * listener, ::wlr_keyboard_repeat_info * rep, Keyboard * keyboard,
	// 		struct ::wlr_keyboard_key_event * event, struct ::wlr_keyboard * kbd)>;

private:
	struct ::wlr_keyboard * _kbd;
	char * _rules;
	char * _model;
	char * _layout;
	char * _variant;
	char * _options;
	struct wlr_keyboard_modifiers * _last_mods;

	std::list<KeyHandler> _on_key;
	std::list<KeyHandler> _on_key_pressed;
	std::list<KeyHandler> _on_key_released;
	std::list<ModHandler> _on_mod;
	// std::list<RepeatHandler> _on_repeat;

	wl_listener _key_listener;
	wl_listener _mod_listener;
	wl_listener _repeat_listener;

public:
	Keyboard(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback);
	~Keyboard();

	bool is_keyboard() const override;
	Keyboard * as_keyboard() override;

	Keyboard & compile_keymap();

	[[nodiscard]] const char * rules() const;
	[[nodiscard]] const char * model() const;
	[[nodiscard]] const char * layout() const;
	[[nodiscard]] const char * variant() const;
	[[nodiscard]] const char * options() const;
	[[nodiscard]] struct wlr_keyboard_modifiers * last_mods();

	Keyboard & set_rules(const char * rules = nullptr);
	Keyboard & set_model(const char * model = nullptr);
	Keyboard & set_layout(const char * layout = nullptr);
	Keyboard & set_variant(const char * variant = nullptr);
	Keyboard & set_options(const char * options = nullptr);

	Keyboard & on_key(const KeyHandler & handler);
	Keyboard & on_key_pressed(const KeyHandler & handler);
	Keyboard & on_key_released(const KeyHandler & handler);
	Keyboard & on_mod(const ModHandler & handler);
	// Keyboard & on_repeat(const RepeatHandler & handler);

private:
	static void _handle_key(struct wl_listener * listener, void * data);
	static void _handle_mod(struct wl_listener * listener, void * data);
	static void _handle_repeat(struct wl_listener * listener, void * data);
};

}
