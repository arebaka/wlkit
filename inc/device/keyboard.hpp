#pragma once

extern "C" {
#include <wlr/types/wlr_keyboard.h>
}

#include "input.hpp"

namespace wlkit {

class Keyboard : public Input {
public:
	typedef uint32_t Keycode;

private:
	struct ::wlr_keyboard * _kbd;
	char * _rules;
	char * _model;
	char * _layout;
	char * _variant;
	char * _options;

	std::list<NotifyHandler> _on_key;

	wl_listener _key_listener;

public:
	Keyboard(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback);
	~Keyboard();

	Keyboard & compile_keymap();

	[[nodiscard]] const char * rules() const;
	[[nodiscard]] const char * model() const;
	[[nodiscard]] const char * layout() const;
	[[nodiscard]] const char * variant() const;
	[[nodiscard]] const char * options() const;

	Keyboard & set_rules(const char * rules = nullptr);
	Keyboard & set_model(const char * model = nullptr);
	Keyboard & set_layout(const char * layout = nullptr);
	Keyboard & set_variant(const char * variant = nullptr);
	Keyboard & set_options(const char * options = nullptr);

	Keyboard & on_key(const NotifyHandler & handler);

private:
	static void _handle_key(struct wl_listener * listener, void * data);
};

}
