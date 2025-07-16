#include "device/pointer.hpp"
#include "server.hpp"
#include "surface.hpp"
#include "seat.hpp"

using namespace wlkit;

Pointer::Pointer(Server * server, struct wlr_input_device * device, const Handler & callback):
Input(server, Type::POINTER, device, callback) {
	_ptr = wlr_pointer_from_input_device(_device);
	if (!_ptr) {
		// TODO error
	}

	_gestures = wlr_pointer_gestures_v1_create(_server->display());
	if (!_gestures) {
		// TODO error
	}

	// _constraints = wlr_pointer_constraints_v1_create(_server->display());
	// if (!_constraints) {
	// 	// TODO error
	// }

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_gestures->events.destroy, &_destroy_listener);
	_motion_listener.notify = _handle_motion;
	wl_signal_add(&_ptr->events.motion, &_motion_listener);
	_button_listener.notify = _handle_button;
	wl_signal_add(&_ptr->events.button, &_button_listener);
	_axis_listener.notify = _handle_axis;
	wl_signal_add(&_ptr->events.axis, &_axis_listener);
	_swipe_begin_listener.notify = _handle_swipe_begin;
	wl_signal_add(&_ptr->events.swipe_begin, &_swipe_begin_listener);
	_swipe_update_listener.notify = _handle_swipe_update;
	wl_signal_add(&_ptr->events.swipe_update, &_swipe_update_listener);
	_swipe_end_listener.notify = _handle_swipe_end;
	wl_signal_add(&_ptr->events.swipe_end, &_swipe_end_listener);
	_pinch_begin_listener.notify = _handle_pinch_begin;
	wl_signal_add(&_ptr->events.pinch_begin, &_pinch_begin_listener);
	_pinch_update_listener.notify = _handle_pinch_update;
	wl_signal_add(&_ptr->events.pinch_update, &_pinch_update_listener);
	_pinch_end_listener.notify = _handle_pinch_end;
	wl_signal_add(&_ptr->events.pinch_end, &_pinch_end_listener);
	_hold_begin_listener.notify = _handle_hold_begin;
	wl_signal_add(&_ptr->events.hold_begin, &_hold_begin_listener);
	_hold_end_listener.notify = _handle_hold_end;
	wl_signal_add(&_ptr->events.hold_end, &_hold_end_listener);
}

Pointer::~Pointer() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	// for (auto & pair : _constraints_by_surface) {
	// 	free(pair->second);
	// }
	// free(_constraints);

	free(_gestures);
	free(_ptr);
}

bool Pointer::is_pointer() const {
	return true;
}

Pointer * Pointer::as_pointer() {
	return this;
}

Pointer & Pointer::send_swipe_begin(NFingers n_fingers) {
	wlr_pointer_gestures_v1_send_swipe_begin(_gestures, _server->seat()->wlr_seat(), 0, n_fingers);
	return *this;
}

Pointer & Pointer::send_swipe_update(Geo dx, Geo dy) {
	wlr_pointer_gestures_v1_send_swipe_update(_gestures, _server->seat()->wlr_seat(), 0, dx, dy);
	return *this;
}

Pointer & Pointer::send_swipe_end(bool cancelled) {
	wlr_pointer_gestures_v1_send_swipe_end(_gestures, _server->seat()->wlr_seat(), 0, cancelled);
	return *this;
}

Pointer & Pointer::send_pinch_begin(NFingers n_fingers) {
	wlr_pointer_gestures_v1_send_pinch_begin(_gestures, _server->seat()->wlr_seat(), 0, n_fingers);
	return *this;
}

Pointer & Pointer::send_pinch_update(Geo dx, Geo dy, PinchScale scale, PinchRotation rotation) {
	wlr_pointer_gestures_v1_send_pinch_update(_gestures, _server->seat()->wlr_seat(), 0, dx, dy, scale, rotation);
	return *this;
}

Pointer & Pointer::send_pinch_end(bool cancelled) {
	wlr_pointer_gestures_v1_send_pinch_end(_gestures, _server->seat()->wlr_seat(), 0, cancelled);
	return *this;
}

Pointer & Pointer::send_hold_begin(NFingers n_fingers) {
	wlr_pointer_gestures_v1_send_hold_begin(_gestures, _server->seat()->wlr_seat(), 0, n_fingers);
	return *this;
}

Pointer & Pointer::send_hold_end(bool cancelled) {
	wlr_pointer_gestures_v1_send_hold_end(_gestures, _server->seat()->wlr_seat(), 0, cancelled);
	return *this;
}
/*
Pointer & Pointer::constraint_for_surface(Surface * surface) {
	auto pair = _constraints_by_surface.find(surface);
	if (pair != _constraints_by_surface.end()) {
		free(pair->second);
	}

	_constraints_by_surface[surface] = wlr_pointer_constraints_v1_constraint_for_surface(
		_constraints, surface->wlr_surface(), _server()->seat()->wlr_seat());

	return *this;
}

Pointer & Pointer::send_constraint_activated(Surface * surface) {
	auto pair = _constraints_by_surface.find(surface);
	if (pair != _constraints_by_surface.end()) {
		wlr_pointer_constraint_v1_send_acitvated(pair->second->wlr_surface, _display->seat()->wlr_seat());
	}

	return *this;
}

Pointer & Pointer::send_constraint_deactivated(Surface * surface) {
	auto pair = _constraints_by_surface.find(surface);
	if (pair != _constraints_by_surface.end()) {
		wlr_pointer_constraint_v1_send_deacitvated(pair->second->wlr_surface, _display->seat()->wlr_seat());
	}
	return *this;
}
*/
struct wlr_pointer * Pointer::wlr_pointer() const {
	return _ptr;
}

Pointer & Pointer::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
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

Pointer & Pointer::on_swipe_begin(const ActionBeginHandler & handler) {
	if (handler) {
		_on_swipe_begin.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_swipe_update(const SwipeUpdateHandler & handler) {
	if (handler) {
		_on_swipe_update.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_swipe_end(const ActionEndHandler & handler) {
	if (handler) {
		_on_swipe_end.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_pinch_begin(const ActionBeginHandler & handler) {
	if (handler) {
		_on_pinch_begin.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_pinch_update(const PinchUpdateHandler & handler) {
	if (handler) {
		_on_pinch_update.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_pinch_end(const ActionEndHandler & handler) {
	if (handler) {
		_on_pinch_end.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_hold_begin(const ActionBeginHandler & handler) {
	if (handler) {
		_on_hold_begin.push_back(std::move(handler));
	}
	return *this;
}

Pointer & Pointer::on_hold_end(const ActionEndHandler & handler) {
	if (handler) {
		_on_hold_end.push_back(std::move(handler));
	}
	return *this;
}

void Pointer::_handle_destroy(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _motion_listener);
	auto event = static_cast<struct wlr_pointer_motion_event*>(data);
	delete pointer;
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

void Pointer::_handle_swipe_begin(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _swipe_begin_listener);
	auto event = static_cast<struct wlr_pointer_swipe_begin_event*>(data);

 	for (auto & cb : pointer->_on_swipe_begin) {
		cb(pointer, event->fingers);
	}
}

void Pointer::_handle_swipe_update(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _swipe_update_listener);
	auto event = static_cast<struct wlr_pointer_swipe_update_event*>(data);

 	for (auto & cb : pointer->_on_swipe_update) {
		cb(pointer, event->fingers, event->dx, event->dy);
	}
}

void Pointer::_handle_swipe_end(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _swipe_end_listener);
	auto event = static_cast<struct wlr_pointer_swipe_end_event*>(data);

 	for (auto & cb : pointer->_on_swipe_end) {
		cb(pointer, event->cancelled);
	}
}

void Pointer::_handle_pinch_begin(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _pinch_begin_listener);
	auto event = static_cast<struct wlr_pointer_pinch_begin_event*>(data);

 	for (auto & cb : pointer->_on_pinch_begin) {
		cb(pointer, event->fingers);
	}
}

void Pointer::_handle_pinch_update(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _pinch_update_listener);
	auto event = static_cast<struct wlr_pointer_pinch_update_event*>(data);

 	for (auto & cb : pointer->_on_pinch_update) {
		cb(pointer, event->fingers, event->dx, event->dy, event->scale, event->rotation);
	}
}

void Pointer::_handle_pinch_end(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _pinch_end_listener);
	auto event = static_cast<struct wlr_pointer_pinch_end_event*>(data);

 	for (auto & cb : pointer->_on_pinch_end) {
		cb(pointer, event->cancelled);
	}
}

void Pointer::_handle_hold_begin(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _hold_begin_listener);
	auto event = static_cast<struct wlr_pointer_hold_begin_event*>(data);

 	for (auto & cb : pointer->_on_hold_begin) {
		cb(pointer, event->fingers);
	}
}

void Pointer::_handle_hold_end(struct wl_listener * listener, void * data) {
	Pointer * pointer = wl_container_of(listener, pointer, _hold_end_listener);
	auto event = static_cast<struct wlr_pointer_hold_end_event*>(data);

 	for (auto & cb : pointer->_on_hold_end) {
		cb(pointer, event->cancelled);
	}
}
