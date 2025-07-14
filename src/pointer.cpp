#include "device/pointer.hpp"

using namespace wlkit;

Pointer::Pointer(Server * server, struct ::wlr_input_device * device, const Handler & callback):
Input(server, Type::POINTER, device, callback) {
	_ptr = wlr_pointer_from_input_device(_device);
	if (!_ptr) {
		// TODO error;
	}

	_motion_listener.notify = _handle_motion;
	wl_signal_add(&_ptr->events.motion, &_motion_listener);
	_button_listener.notify = _handle_button;
	wl_signal_add(&_ptr->events.button, &_button_listener);
	_axis_listener.notify = _handle_axis;
	wl_signal_add(&_ptr->events.axis, &_axis_listener);
}

Pointer::~Pointer() {}

bool Pointer::is_pointer() const {
	return true;
}

Pointer * Pointer::as_pointer() {
	return this;
}

Pointer & Pointer::on_motion(const MotionHandler & handler) {
	if (handler) {
		_on_motion.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_button(const ButtonHandler & handler) {
	if (handler) {
		_on_button.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_axis(const AxisHandler & handler) {
	if (handler) {
		_on_axis.push_back(std::move(handler));
	}
	return *this;
}

void Pointer::_handle_motion(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _motion_listener);
	auto event = static_cast<struct wlr_pointer_motion_event*>(data);

 	for (auto & cb : pointer->_on_motion) {
		cb(pointer, event->delta_x, event->delta_y, event->unaccel_dx, event->unaccel_dy);
	}
}

void Pointer::_handle_button(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _button_listener);
	auto event = static_cast<struct wlr_pointer_button_event*>(data);

 	for (auto & cb : pointer->_on_button) {
		cb(pointer, event->button, event->state == 1);
	}
}

void Pointer::_handle_axis(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _axis_listener);
	auto event = static_cast<struct wlr_pointer_axis_event*>(data);

 	for (auto & cb : pointer->_on_axis) {
		cb(pointer, event->source, event->orientation, event->relative_direction, event->delta, event->delta_discrete);
	}
}
