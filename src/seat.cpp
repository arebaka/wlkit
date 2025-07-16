#include "seat.hpp"
#include "surface.hpp"
#include "device/pointer.hpp"
#include "device/keyboard.hpp"
#include <wayland-util.h>

using namespace wlkit;

Seat::Seat(char * name, const Handler & callback):
_name(name) {
	_display = wl_display_create();
	_wlr_seat = wlr_seat_create(_display, strdup(_name ? _name : ""));
	if (!_wlr_seat) {
		// TODO error
	}

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_wlr_seat->events.destroy, &_destroy_listener);
	_pointer_grab_begin_listener.notify = _handle_pointer_grab_begin;
	wl_signal_add(&_wlr_seat->events.pointer_grab_begin, &_pointer_grab_begin_listener);
	_pointer_grab_end_listener.notify = _handle_pointer_grab_end;
	wl_signal_add(&_wlr_seat->events.pointer_grab_end, &_pointer_grab_end_listener);
	_keyboard_grab_begin_listener.notify = _handle_keyboard_grab_begin;
	wl_signal_add(&_wlr_seat->events.keyboard_grab_begin, &_keyboard_grab_begin_listener);
	_keyboard_grab_end_listener.notify = _handle_keyboard_grab_end;
	wl_signal_add(&_wlr_seat->events.keyboard_grab_end, &_keyboard_grab_end_listener);
	_touch_grab_begin_listener.notify = _handle_touch_grab_begin;
	wl_signal_add(&_wlr_seat->events.touch_grab_begin, &_touch_grab_begin_listener);
	_touch_grab_end_listener.notify = _handle_touch_grab_end;
	wl_signal_add(&_wlr_seat->events.touch_grab_end, &_touch_grab_end_listener);
	_request_set_cursor_listener.notify = _handle_request_set_cursor;
	wl_signal_add(&_wlr_seat->events.request_set_cursor, &_request_set_cursor_listener);
	_request_set_selection_listener.notify = _handle_request_set_selection;
	wl_signal_add(&_wlr_seat->events.request_set_selection, &_request_set_selection_listener);
	_set_selection_listener.notify = _handle_set_selection;
	wl_signal_add(&_wlr_seat->events.set_selection, &_set_selection_listener);
	_request_set_primary_selection_listener.notify = _handle_request_set_primary_selection;
	wl_signal_add(&_wlr_seat->events.request_set_primary_selection, &_request_set_primary_selection_listener);
	_set_primary_selection_listener.notify = _handle_set_primary_selection;
	wl_signal_add(&_wlr_seat->events.set_primary_selection, &_set_primary_selection_listener);
	_request_start_drag_listener.notify = _handle_request_start_drag;
	wl_signal_add(&_wlr_seat->events.request_start_drag, &_request_start_drag_listener);
	_start_drag_listener.notify = _handle_start_drag;
	wl_signal_add(&_wlr_seat->events.start_drag, &_start_drag_listener);

	if (callback) {
		_on_create.push_back(callback);
		callback(this);
	}
}

Seat::~Seat() {
	wlr_seat_destroy(_wlr_seat);
	free(_name);
}

void Seat::set_capabilities(Capabilities capabilities) {
	wlr_seat_set_capabilities(_wlr_seat, capabilities);
}

void Seat::set_name(char * name) {
	wlr_seat_set_name(_wlr_seat, name);
	free(_name);
	_name = strdup(name ? name : "");
}

bool Seat::surface_has_pointer_focus(Surface * surface) const {
	return wlr_seat_pointer_surface_has_focus(_wlr_seat, surface->wlr_surface());
}

void Seat::enter_pointer(Surface * surface, Geo sx, Geo sy) {
	wlr_seat_pointer_enter(_wlr_seat, surface->wlr_surface(), sx, sy);
}

void Seat::clear_pointer_focus() {
	wlr_seat_pointer_clear_focus(_wlr_seat);
}

void Seat::send_pointer_motion(Time time, Geo sx, Geo sy) {
	wlr_seat_pointer_send_motion(_wlr_seat, time, sx, sy);
}

Seat::Serial Seat::send_pointer_button(Time time, Pointer::Button button, bool state) {
	return wlr_seat_pointer_send_button(_wlr_seat, time, button,
		state ? WL_POINTER_BUTTON_STATE_RELEASED : WL_POINTER_BUTTON_STATE_PRESSED);
}

void Seat::send_pointer_axis(Time time, Pointer::AxisOrientation orientation,
	Pointer::AxisDelta value, Pointer::AxisDeltaDiscrete value_discrete,
	Pointer::AxisSource source, Pointer::AxisRelativeDirection relative_direction
) {
		wlr_seat_pointer_send_axis(_wlr_seat, time, orientation, value, value_discrete, source, relative_direction);
}

void Seat::send_pointer_frame() {
	wlr_seat_pointer_send_frame(_wlr_seat);
}

void Seat::warp_pointer(Geo sx, Geo sy) {
	wlr_seat_pointer_warp(_wlr_seat, sx, sy);
}

void Seat::start_pointer_grab(wlr_seat_pointer_grab * grab) {
	wlr_seat_pointer_start_grab(_wlr_seat, grab);
}

void Seat::end_pointer_grab() {
	wlr_seat_pointer_end_grab(_wlr_seat);
}

bool Seat::has_pointer_grab() {
	return wlr_seat_pointer_has_grab(_wlr_seat);
}

void Seat::send_keyboard_key(Time time, Keyboard::Keycode key, bool state) {
	wlr_seat_keyboard_send_key(_wlr_seat, time, key, state);
}

void Seat::send_keyboard_mod(wlr_keyboard_modifiers * modifiers) {
	wlr_seat_keyboard_send_modifiers(_wlr_seat, modifiers);
}

void Seat::enter_keyboard(Surface * surface,
	Keyboard::Keycode keycodes[], Keyboard::Keycode n_keycodes, wlr_keyboard_modifiers* modifiers
) {
	wlr_seat_keyboard_enter(_wlr_seat, surface->wlr_surface(), keycodes, n_keycodes, modifiers);
}

void Seat::clear_keyboard_focus() {
	wlr_seat_keyboard_clear_focus(_wlr_seat);
}

void Seat::start_keyboard_grab(wlr_seat_keyboard_grab * grab) {
	wlr_seat_keyboard_start_grab(_wlr_seat, grab);
}

void Seat::end_keyboard_grab() {
	wlr_seat_keyboard_end_grab(_wlr_seat);
}

bool Seat::has_keyboard_grab() {
	return wlr_seat_keyboard_has_grab(_wlr_seat);
}
/*
wlr_touch_point * Seat::get_touch_point(int32_t touch_id) const {
	if (!_wlr_seat) return nullptr;
	return wlr_seat_touch_get_point(_wlr_seat, touch_id);
}

void Seat::focus_touch_point(wlr_surface* surface, uint32_t time_msec, int32_t touch_id, double sx, double sy) {
	if (_wlr_seat) {
		wlr_seat_touch_point_focus(_wlr_seat, surface, time_msec, touch_id, sx, sy);
	}
}

void Seat::clear_touch_point_focus(uint32_t time_msec, int32_t touch_id) {
	if (_wlr_seat) {
		wlr_seat_touch_point_clear_focus(_wlr_seat, time_msec, touch_id);
	}
}

uint32_t Seat::send_touch_down(wlr_surface* surface, uint32_t time_msec, int32_t touch_id, double sx, double sy) {
	if (!_wlr_seat) return 0;
	return wlr_seat_touch_send_down(_wlr_seat, surface, time_msec, touch_id, sx, sy);
}

uint32_t Seat::send_touch_up(uint32_t time_msec, int32_t touch_id) {
	if (!_wlr_seat) return 0;
	return wlr_seat_touch_send_up(_wlr_seat, time_msec, touch_id);
}

void Seat::send_touch_motion(uint32_t time_msec, int32_t touch_id, double sx, double sy) {
	if (_wlr_seat) {
		wlr_seat_touch_send_motion(_wlr_seat, time_msec, touch_id, sx, sy);
	}
}

void Seat::send_touch_cancel(wlr_seat_client* seat_client) {
	if (_wlr_seat) {
		wlr_seat_touch_send_cancel(_wlr_seat, seat_client);
	}
}

int Seat::count_touch_points() const {
	if (!_wlr_seat) return 0;
	return wlr_seat_touch_num_points(_wlr_seat);
}

void Seat::start_touch_grab(wlr_seat_touch_grab* grab) {
	if (_wlr_seat) {
		wlr_seat_touch_start_grab(_wlr_seat, grab);
	}
}

void Seat::end_touch_grab() {
	if (_wlr_seat) {
		wlr_seat_touch_end_grab(_wlr_seat);
	}
}

bool Seat::has_touch_grab() const {
	if (!_wlr_seat) return false;
	return wlr_seat_touch_has_grab(_wlr_seat);
}
*/
bool Seat::validate_pointer_grab_serial(Surface * origin, Serial serial) const {
	return wlr_seat_validate_pointer_grab_serial(_wlr_seat, origin->wlr_surface(), serial);
}

bool Seat::validate_touch_grab_serial(Surface * origin, Serial serial, wlr_touch_point * point) const {
	return wlr_seat_validate_touch_grab_serial(_wlr_seat, origin->wlr_surface(), serial, &point);
}

Seat::Serial Seat::next_client_serial(struct wlr_seat_client * client) const {
	return wlr_seat_client_next_serial(client);
}

bool Seat::validate_client_event_serial(struct wlr_seat_client * client, Serial serial) const {
	return wlr_seat_client_validate_event_serial(client, serial);
}

bool Seat::accepts_touch(Surface * surface) const {
	return wlr_surface_accepts_touch(surface->wlr_surface(), _wlr_seat);
}

struct wl_display * Seat::display() const {
	return _display;
}

struct wlr_seat * Seat::wlr_seat() const {
	return _wlr_seat;
}

struct wlr_seat_client * Seat::wlr_seat_client() const {
	return _wlr_seat_client;
}

Seat & Seat::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_pointer_grab_begin(const Handler & handler) {
	if (handler) {
		_on_pointer_grab_begin.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_pointer_grab_end(const Handler & handler) {
	if (handler) {
		_on_pointer_grab_end.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_keyboard_grab_begin(const Handler & handler) {
	if (handler) {
		_on_keyboard_grab_begin.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_keyboard_grab_end(const Handler & handler) {
	if (handler) {
		_on_keyboard_grab_end.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_touch_grab_begin(const Handler & handler) {
	if (handler) {
		_on_touch_grab_begin.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_touch_grab_end(const Handler & handler) {
	if (handler) {
		_on_touch_grab_end.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_request_set_cursor(const RequestSetCursorHandler & handler) {
	if (handler) {
		_on_request_set_cursor.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_request_set_selection(const RequestSetSelectionHandler & handler) {
	if (handler) {
		_on_request_set_selection.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_set_selection(const Handler & handler) {
	if (handler) {
		_on_set_selection.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_request_set_primary_selection(const RequestSetPrimarySelectionHandler & handler) {
	if (handler) {
		_on_request_set_primary_selection.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_set_primary_selection(const Handler & handler) {
	if (handler) {
		_on_set_primary_selection.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_request_start_drag(const RequestStartDragHandler & handler) {
	if (handler) {
		_on_request_start_drag.push_back(handler);
	}
	return *this;
}

Seat & Seat::on_start_drag(const StartDragHandler & handler) {
	if (handler) {
		_on_start_drag.push_back(handler);
	}
	return *this;
}

void Seat::_handle_destroy(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	delete seat;
}

void Seat::_handle_pointer_grab_begin(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}
}

void Seat::_handle_pointer_grab_end(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_keyboard_grab_begin(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_keyboard_grab_end(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_touch_grab_begin(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_touch_grab_end(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_request_set_cursor(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _request_set_cursor_listener);
	auto event = static_cast<struct wlr_seat_pointer_request_set_cursor_event*>(data);
	for (auto & cb : seat->_on_request_set_cursor) {
		cb(seat, event);
	}

}

void Seat::_handle_request_set_selection(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _request_set_selection_listener);
	auto event = static_cast<struct wlr_seat_request_set_selection_event*>(data);
	for (auto & cb : seat->_on_request_set_selection) {
		cb(seat, event);
	}

}

void Seat::_handle_set_selection(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _destroy_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_request_set_primary_selection(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _request_set_primary_selection_listener);
	auto event = static_cast<struct wlr_seat_request_set_primary_selection_event*>(data);
	for (auto & cb : seat->_on_request_set_primary_selection) {
		cb(seat, event);
	}

}

void Seat::_handle_set_primary_selection(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _set_primary_selection_listener);
	for (auto & cb : seat->_on_pointer_grab_begin) {
		cb(seat);
	}

}

void Seat::_handle_request_start_drag(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _request_start_drag_listener);
	auto event = static_cast<struct wlr_seat_request_start_drag_event*>(data);
	for (auto & cb : seat->_on_request_start_drag) {
		cb(seat, event);
	}

}

void Seat::_handle_start_drag(struct ::wl_listener * listener, void * data) {
	Seat * seat = wl_container_of(listener, seat, _start_drag_listener);
	auto drag = static_cast<struct wlr_drag*>(data);
	for (auto & cb : seat->_on_start_drag) {
		cb(seat, drag);
	}
}
