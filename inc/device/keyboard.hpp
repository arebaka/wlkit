#pragma once

extern "C" {
#include <wlr/types/wlr_keyboard.h>
}

#include "../input.hpp"
#include "pointer.hpp"

namespace wlkit {

class Keyboard : public Input {
public:
	struct RepeatInfo {
		int32_t rate;
		int32_t delay;
	};

	enum class Mod {
		SHIFT = 1 << 0,
		CAPS = 1 << 1,
		CONTROL = 1 << 2,
		ALT = 1 << 3,
		META = 1 << 3,
		MOD2 = 1 << 4,
		MOD3 = 1 << 5,
		MOD4 = 1 << 6,
		SUPER = 1 << 6,
		MOD5 = 1 << 7,
	};

	using Keycode = uint32_t;
	using ModMask = uint32_t;
	using LEDsMask = uint32_t;
	using RepeatRate = int32_t;
	using RepeatDelay = int32_t;

	using KeyHandler = std::function<
		void(Keyboard * keyboard, Keycode keycode, bool state)>;
	using KeyStateHandler = std::function<
		void(Keyboard * keyboard, Keycode keycode)>;
	using ModHandler = std::function<
		void(Keyboard * keyboard, struct ::wlr_keyboard_modifiers * mods)>;
	using RepeatHandler = std::function<
		void(Keyboard * keyboard, RepeatRate rate, RepeatDelay delay)>;

private:
	struct ::wlr_keyboard * _kbd;
	char * _rules;
	char * _model;
	char * _layout;
	char * _variant;
	char * _options;

	std::list<KeyHandler> _on_key;
	std::list<KeyStateHandler> _on_key_pressed;
	std::list<KeyStateHandler> _on_key_released;
	std::list<ModHandler> _on_mod;
	std::list<RepeatHandler> _on_repeat;

	struct ::wl_listener _key_listener;
	struct ::wl_listener _mod_listener;
	struct ::wl_listener _repeat_listener;

public:
	Keyboard(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback = nullptr);
	~Keyboard() override;

	bool is_keyboard() const override;
	Keyboard * as_keyboard() override;

	Keyboard & compile_keymap();
	Keyboard & keysym_to_pointer_button(xkb_keysym_t keysym);
	Keyboard & keysym_to_pointer_motion(xkb_keysym_t keysym,
		Pointer::MotionOrientation dx, Pointer::MotionOrientation dy);
	Keyboard & set_repeat_info(RepeatInfo info);
	Keyboard & set_leds(LEDsMask mask);

	[[nodiscard]] struct ::wlr_keyboard * wlr_keyboard() const;
	[[nodiscard]] const char * rules() const;
	[[nodiscard]] const char * model() const;
	[[nodiscard]] const char * layout() const;
	[[nodiscard]] const char * variant() const;
	[[nodiscard]] const char * options() const;

	ModMask mods() const;

	Keyboard & set_rules(const char * rules = nullptr);
	Keyboard & set_model(const char * model = nullptr);
	Keyboard & set_layout(const char * layout = nullptr);
	Keyboard & set_variant(const char * variant = nullptr);
	Keyboard & set_options(const char * options = nullptr);

	Keyboard & on_key(const KeyHandler & handler);
	Keyboard & on_key_pressed(const KeyStateHandler & handler);
	Keyboard & on_key_released(const KeyStateHandler & handler);
	Keyboard & on_mod(const ModHandler & handler);
	Keyboard & on_repeat(const RepeatHandler & handler);

private:
	static void _handle_key(struct ::wl_listener * listener, void * data);
	static void _handle_mod(struct ::wl_listener * listener, void * data);
	static void _handle_repeat(struct ::wl_listener * listener, void * data);
};

}
