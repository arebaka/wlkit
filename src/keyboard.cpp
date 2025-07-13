#include "device/keyboard.hpp"

using namespace wlkit;

Keyboard::Keyboard(Server * server, struct ::wlr_input_device * device, const Handler & callback):
Input(server, Type::KEYBOARD, device, callback),
_rules(nullptr), _model(nullptr), _layout(nullptr), _variant(nullptr), _options(nullptr) {
	_kbd = wlr_keyboard_from_input_device(_device);
	if (!_kbd) {
		// TODO error;
	}
	_key_listener.notify = _handle_key;
	wl_signal_add(&_kbd->events.key, &_key_listener);
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

Keyboard & Keyboard::compile_keymap() {
		struct xkb_rule_names rules{};
		rules.rules = _rules ? _rules : getenv("XKB_DEFAULT_RULES");
		rules.model = _model ? _model : getenv("XKB_DEFAULT_MODEL");
		rules.layout = _layout ? _layout : getenv("XKB_DEFAULT_LAYOUT");
		rules.variant = _variant ? _variant : getenv("XKB_DEFAULT_VARIANT");
		rules.options = _options ? _options : getenv("XKB_DEFAULT_OPTIONS");

		struct xkb_context * context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		if (!context) {
			// TODO error
		}
		struct xkb_keymap * keymap = xkb_map_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);
		if (!keymap) {
			// TODO error
		}

		wlr_keyboard_set_keymap(_kbd, keymap);

		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
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

Keyboard & Keyboard::set_rules(const char * rules) {
	_rules = strdup(rules ? rules : getenv("XKB_DEFAULT_RULES"));
}

Keyboard & Keyboard::set_model(const char * model) {
	_model = strdup(model ? model : getenv("XKB_DEFAULT_MODEL"));
}

Keyboard & Keyboard::set_layout(const char * layout) {
	_layout = strdup(layout ? layout : getenv("XKB_DEFAULT_LAYOUT"));
}

Keyboard & Keyboard::set_variant(const char * variant) {
	_variant = strdup(variant ? variant : getenv("XKB_DEFAULT_VARIANT"));
}

Keyboard & Keyboard::set_options(const char * options) {
	_options = strdup(options ? options : getenv("XKB_DEFAULT_OPTIONS"));
}

Keyboard & Keyboard::on_key(const NotifyHandler & handler) {
	if (handler) {
		_on_key.push_back(handler);
	}
}

void Keyboard::_handle_key(struct wl_listener * listener, void * data) {
	Keyboard * keyboard = wl_container_of(listener, keyboard, _key_listener);
	auto event = static_cast<struct wlr_event_keyboard_key*>(data);

	Object object{ .keyboard = keyboard };
	for (auto & cb : keyboard->_on_key) {
		cb(listener, data, object);
	}
}
