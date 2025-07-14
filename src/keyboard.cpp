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

Keyboard & Keyboard::keysym_to_pointer_button(xkb_keysym_t keysym) {
	wlr_keyboard_keysym_to_pointer_button(keysym);
}

Keyboard & Keyboard::keysym_to_pointer_motion(xkb_keysym_t keysym,
	Pointer::MotionOrientation dx, Pointer::MotionOrientation dy
) {
	wlr_keyboard_keysym_to_pointer_motion(keysym, static_cast<int*>(&dx), static_cast<int*>(&dy));
}

Keyboard & Keyboard::set_repeat_info(RepeatInfo info) {
	wlr_keyboard_set_repeat_info(_kbd, info.rate, info.delay);
}

Keyboard & Keyboard::set_leds(LEDsMask mask) {
	wlr_keyboard_led_update(_kbd, mask);
}

struct wlr_keyboard * Keyboard::wlr_keyboard() const {
	return _kbd;
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

Keyboard::ModMask Keyboard::mods() const {
	return wlr_keyboard_get_modifiers(_kbd);
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

Keyboard & Keyboard::on_key_pressed(const KeyStateHandler & handler) {
	if (handler) {
		_on_key_pressed.push_back(std::move(handler));
	}
	return *this;
}

Keyboard & Keyboard::on_key_released(const KeyStateHandler & handler) {
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

Keyboard & Keyboard::on_repeat(const RepeatHandler & handler) {
	if (handler) {
		_on_repeat.push_back(std::move(handler));
	}
	return *this;
}

void Keyboard::_handle_key(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _key_listener);
	auto event = static_cast<struct wlr_keyboard_key_event*>(data);
	if (!keyboard->_kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_key) {
		cb(keyboard, event->keycode, event->state);
	}

	auto handlers = event->state == WL_KEYBOARD_KEY_STATE_PRESSED
		? keyboard->_on_key_pressed : keyboard->_on_key_released;
	for (auto & cb : handlers) {
		cb(keyboard, event->keycode);
	}
}

void Keyboard::_handle_mod(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _mod_listener);
	auto mods  = static_cast<struct wlr_keyboard_modifiers*>(data);
	if (!keyboard->_kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_mod) {
		cb(keyboard, mods);
	}
}

void Keyboard::_handle_repeat(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _repeat_listener);
	auto info = static_cast<RepeatInfo*>(data);
	if (!keyboard->_kbd->xkb_state) {
		return;
	}

 	for (auto & cb : keyboard->_on_repeat) {
		cb(keyboard, *info);
	}
}
