#include "device/switch.hpp"

using namespace wlkit;

Switch::Switch(Server * server, struct wlr_input_device * device, const Handler & callback):
Input(server, Type::SWITCH, device, callback) {
	_wlr_switch = wlr_switch_from_input_device(_device);
	if (!_wlr_switch) {
		// TODO error;
	}

	_toggle_listener.notify = _handle_toggle;
	wl_signal_add(&_wlr_switch->events.toggle, &_toggle_listener);
}

Switch::~Switch() {}

bool Switch::is_switch() const {
	return true;
}

Switch * Switch::as_switch() {
	return this;
}

Switch & Switch::on_toggle(const ToggleHandler & handler) {
	if (handler) {
		_on_toggle.push_back(std::move(handler));
	}
	return *this;
}

Switch & Switch::on_toggle_on(const ToggleStateHandler & handler) {
	if (handler) {
		_on_toggle_on.push_back(std::move(handler));
	}
	return *this;
}

Switch & Switch::on_toggle_off(const ToggleStateHandler & handler) {
	if (handler) {
		_on_toggle_off.push_back(std::move(handler));
	}
	return *this;
}

void Switch::_handle_toggle(struct wl_listener * listener, void * data) {
	Switch * switch_ = wl_container_of(listener, switch_, _toggle_listener);
	auto event = static_cast<struct wlr_switch_toggle_event*>(data);

	SwitchType type;
	switch (event->switch_type) {
	case WLR_SWITCH_TYPE_LID:
		type = SwitchType::LID;
		break;
	case WLR_SWITCH_TYPE_TABLET_MODE:
		type = SwitchType::TABLET_MODE;
		break;
	defaut:
		return;
	}

 	for (auto & cb : switch_->_on_toggle) {
		cb(switch_, type, event->switch_state);
	}

	auto handlers = event->switch_state == WLR_SWITCH_STATE_ON
		? switch_->_on_toggle_on : switch_->_on_toggle_off;
	for (auto & cb : handlers) {
		cb(switch_, type);
	}
}
