#include "device/keyboard.hpp"

#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>

using namespace wlkit;

Keyboard::Keyboard(Server * server, struct ::wlr_input_device * device, const Handler & callback):
Input(server, Type::KEYBOARD, device, callback),
_rules(nullptr), _model(nullptr), _layout(nullptr), _variant(nullptr), _options(nullptr) {
	_kbd = wlr_keyboard_from_input_device(_device);
	if (!_kbd) {
		// TODO error;
	}

	memset(&_last_mods, 0, sizeof(_last_mods));

	compile_keymap();

	_key_listener.notify = _handle_key;
	wl_signal_add(&_kbd->events.key, &_key_listener);
	_mod_listener.notify = _handle_mod;
	wl_signal_add(&_kbd->events.modifiers, &_mod_listener);
	_repeat_listener.notify = _handle_repeat;
	wl_signal_add(&_kbd->events.repeat_info, &_repeat_listener);
}

Keyboard::~Keyboard() {
	if (_rules) {
		free(_rules);
	}
	if (_model) {
		free(_model);
	}
	if (_layout) {
		free(_layout);
	}
	if (_variant) {
		free(_variant);
	}
	if (_options) {
		free(_options);
	}
}

bool Keyboard::is_keyboard() const {
	return true;
}

Keyboard * Keyboard::as_keyboard() {
	return this;
}

Keyboard & Keyboard::compile_keymap() {
	struct xkb_rule_names rules{
		.rules = _rules ? _rules : getenv("XKB_DEFAULT_RULES"),
		.model = _model ? _model : getenv("XKB_DEFAULT_MODEL"),
		.layout = _layout ? _layout : getenv("XKB_DEFAULT_LAYOUT"),
		.variant = _variant ? _variant : getenv("XKB_DEFAULT_VARIANT"),
		.options = _options ? _options : getenv("XKB_DEFAULT_OPTIONS")
	};

	auto context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (!context) {
		// TODO error
	}
	auto keymap = xkb_map_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
	if (!keymap) {
		// TODO error
	}

	wlr_keyboard_set_keymap(_kbd, keymap);

	xkb_keymap_unref(keymap);
	xkb_context_unref(context);

	return *this;
}

const char * Keyboard::rules() const {
	return _rules;
}

const char * Keyboard::model() const {
	return _model;
}

const char * Keyboard::layout() const {
	return _layout;
}

const char * Keyboard::variant() const {
	return _variant;
}

const char * Keyboard::options() const {
	return _options;
}

struct wlr_keyboard_modifiers * Keyboard::last_mods() {
	return _last_mods;
}

Keyboard & Keyboard::set_rules(const char * rules) {
	_rules = strdup(rules ? rules : getenv("XKB_DEFAULT_RULES"));
	return *this;
}

Keyboard & Keyboard::set_model(const char * model) {
	_model = strdup(model ? model : getenv("XKB_DEFAULT_MODEL"));
	return *this;
}

Keyboard & Keyboard::set_layout(const char * layout) {
	_layout = strdup(layout ? layout : getenv("XKB_DEFAULT_LAYOUT"));
	return *this;
}

Keyboard & Keyboard::set_variant(const char * variant) {
	_variant = strdup(variant ? variant : getenv("XKB_DEFAULT_VARIANT"));
	return *this;
}

Keyboard & Keyboard::set_options(const char * options) {
	_options = strdup(options ? options : getenv("XKB_DEFAULT_OPTIONS"));
	return *this;
}

Keyboard & Keyboard::on_key(const KeyHandler & handler) {
	if (handler) {
		_on_key.push_back(std::move(handler));
	}
	return *this;
}

Keyboard & Keyboard::on_key_pressed(const KeyHandler & handler) {
	if (handler) {
		_on_key_pressed.push_back(std::move(handler));
	}
	return *this;
}

Keyboard & Keyboard::on_key_released(const KeyHandler & handler) {
	if (handler) {
		_on_key_released.push_back(std::move(handler));
	}
	return *this;
}

Keyboard & Keyboard::on_mod(const ModHandler & handler) {
	if (handler) {
		_on_mod.push_back(std::move(handler));
	}
	return *this;
}

// Keyboard & Keyboard::on_repeat(const RepeatHandler & handler) {
// 	if (handler) {
// 		_on_repeat.push_back(std::move(handler));
// 	}
// 	return *this;
// }

void Keyboard::_handle_key(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _key_listener);
	auto event = static_cast<struct wlr_keyboard_key_event*>(data);
	auto kbd = wlr_keyboard_from_input_device(keyboard->device());
	if (!kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_key) {
		cb(event, keyboard, kbd);
	}

	auto handlers = event->state == WL_KEYBOARD_KEY_STATE_PRESSED
		? keyboard->_on_key_pressed : keyboard->_on_key_released;
	for (auto & cb : handlers) {
		cb(event, keyboard, kbd);
	}
}

void Keyboard::_handle_mod(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _mod_listener);
	auto * mods  = static_cast<struct wlr_keyboard_modifiers*>(data);
	auto kbd = wlr_keyboard_from_input_device(keyboard->device());
	if (!kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_mod) {
		cb(mods, keyboard, kbd);
	}

	keyboard->_last_mods = mods;
}

void Keyboard::_handle_repeat(struct wl_listener * listener, void * data) {
/*	Keyboard * keyboard = wl_container_of(listener, keyboard, _repeat_listener);
	auto event = static_cast<struct wlr_keyboard_key_event*>(data);
	auto * rep  = static_cast<struct wlr_keyboard_repeat_info*>(data);
	auto kbd = wlr_keyboard_from_input_device(keyboard->device());
	if (!kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_repeat) {
		cb(listener, rep, keyboard, event, kbd);
	}
*/}

