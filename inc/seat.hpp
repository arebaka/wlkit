#pragma once

extern "C" {
#include <wlr/types/wlr_seat.h>
}

#include "common.hpp"
#include "device/pointer.hpp"
#include "device/keyboard.hpp"

namespace wlkit {

class Seat {
public:
	using Capabilities = uint32_t;
	using Serial = uint32_t;

	using Handler = std::function<void(Seat*)>;
	using RequestSetCursorHandler = std::function<
		void(Seat * seat, struct ::wlr_seat_pointer_request_set_cursor_event * event)>;
	using RequestSetSelectionHandler = std::function<
		void(Seat * seat, struct ::wlr_seat_request_set_selection_event * event)>;
	using RequestSetPrimarySelectionHandler = std::function<
		void(Seat * seat, struct ::wlr_seat_request_set_primary_selection_event * event)>;
	using RequestStartDragHandler = std::function<
		void(Seat * seat, struct ::wlr_seat_request_start_drag_event * event)>;
	using StartDragHandler = std::function<
		void(Seat * seat, struct ::wlr_drag * drag)>;


private:
	char * _name;
	struct ::wl_display * _display;
	struct ::wlr_seat * _wlr_seat;
	struct ::wlr_seat_client * _wlr_seat_client;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	std::list<Handler> _on_pointer_grab_begin;
	std::list<Handler> _on_pointer_grab_end;
	std::list<Handler> _on_keyboard_grab_begin;
	std::list<Handler> _on_keyboard_grab_end;
	std::list<Handler> _on_touch_grab_begin;
	std::list<Handler> _on_touch_grab_end;
	std::list<RequestSetCursorHandler> _on_request_set_cursor;
	std::list<RequestSetSelectionHandler> _on_request_set_selection;
	std::list<Handler> _on_set_selection;
	std::list<RequestSetPrimarySelectionHandler> _on_request_set_primary_selection;
	std::list<Handler> _on_set_primary_selection;
	std::list<RequestStartDragHandler> _on_request_start_drag;
	std::list<StartDragHandler> _on_start_drag;

	struct ::wl_listener _destroy_listener;
	struct ::wl_listener _pointer_grab_begin_listener;
	struct ::wl_listener _pointer_grab_end_listener;
	struct ::wl_listener _keyboard_grab_begin_listener;
	struct ::wl_listener _keyboard_grab_end_listener;
	struct ::wl_listener _touch_grab_begin_listener;
	struct ::wl_listener _touch_grab_end_listener;
	struct ::wl_listener _request_set_cursor_listener;
	struct ::wl_listener _request_set_selection_listener;
	struct ::wl_listener _set_selection_listener;
	struct ::wl_listener _request_set_primary_selection_listener;
	struct ::wl_listener _set_primary_selection_listener;
	struct ::wl_listener _request_start_drag_listener;
	struct ::wl_listener _start_drag_listener;

public:
	Seat(
		char * name,
		const Handler & callback = nullptr);
	~Seat();

	void set_capabilities(Capabilities capabilities);
	void set_name(char * name);

	bool surface_has_pointer_focus(Surface * surface) const;
	void enter_pointer(Surface * surface, double sx, double sy);
	void clear_pointer_focus();
	void send_pointer_motion(Time time, Geo sx, Geo sy);
	Serial send_pointer_button(Time time, Pointer::Button button, bool state);
	void send_pointer_axis(Time time, Pointer::AxisOrientation orientation,
		Pointer::AxisDelta value, Pointer::AxisDeltaDiscrete value_discrete,
		Pointer::AxisSource source, Pointer::AxisRelativeDirection relative_direction);
	void send_pointer_frame();
	void warp_pointer(Geo sx, Geo sy);
	void start_pointer_grab(struct ::wlr_seat_pointer_grab * grab);
	void end_pointer_grab();
	bool has_pointer_grab();

	void send_keyboard_key(Time time, Keyboard::Keycode key, bool state);
	void send_keyboard_mod(struct ::wlr_keyboard_modifiers * modifiers);
	void enter_keyboard(Surface * surface,
		Keyboard::Keycode * keycodes, Keyboard::Keycode n_keycodes, struct ::wlr_keyboard_modifiers * modifiers);
	void clear_keyboard_focus();
	void start_keyboard_grab(struct ::wlr_seat_keyboard_grab * grab);
	void end_keyboard_grab();
	bool has_keyboard_grab();

	// wlr_touch_point * get_touch_point(int32_t touch_id) const;
	// void focus_touch_point(Surface * surface, uint32_t time_msec, int32_t touch_id, double sx, double sy);
	// void clear_touch_point_focus(uint32_t time_msec, int32_t touch_id);
	// uint32_t send_touch_down(struct Surface * surface, uint32_t time_msec, int32_t touch_id, double sx, double sy);
	// uint32_t send_touch_up(uint32_t time_msec, int32_t touch_id);
	// void send_touch_motion(uint32_t time_msec, int32_t touch_id, double sx, double sy);
	// void send_touch_cancel(struct ::wlr_seat_client * seat_client);
	// int count_touch_points();
	// void start_touch_grab(struct ::wlr_seat_touch_grab * grab);
	// void end_touch_grab();
	// bool has_touch_grab();

	bool validate_pointer_grab_serial(Surface * origin, Serial serial) const;
	bool validate_touch_grab_serial(Surface * origin, Serial serial, wlr_touch_point * point = nullptr) const;
	Serial next_client_serial(struct ::wlr_seat_client * client) const;
	bool validate_client_event_serial(struct ::wlr_seat_client * client, Serial serial) const;
	bool accepts_touch(Surface * surface) const;

	[[nodiscard]] struct ::wl_display * display() const;
	[[nodiscard]] struct ::wlr_seat * wlr_seat() const;
	[[nodiscard]] struct ::wlr_seat_client * wlr_seat_client() const;

	Seat & on_destroy(const Handler & handler);
	Seat & on_pointer_grab_begin(const Handler & handler);
	Seat & on_pointer_grab_end(const Handler & handler);
	Seat & on_keyboard_grab_begin(const Handler & handler);
	Seat & on_keyboard_grab_end(const Handler & handler);
	Seat & on_touch_grab_begin(const Handler & handler);
	Seat & on_touch_grab_end(const Handler & handler);
	Seat & on_request_set_cursor(const RequestSetCursorHandler & handler);
	Seat & on_request_set_selection(const RequestSetSelectionHandler & handler);
	Seat & on_set_selection(const Handler & handler);
	Seat & on_request_set_primary_selection(const RequestSetPrimarySelectionHandler & handler);
	Seat & on_set_primary_selection(const Handler & handler);
	Seat & on_request_start_drag(const RequestStartDragHandler & handler);
	Seat & on_start_drag(const StartDragHandler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
	static void _handle_pointer_grab_begin(struct ::wl_listener * listener, void * data);
	static void _handle_pointer_grab_end(struct ::wl_listener * listener, void * data);
	static void _handle_keyboard_grab_begin(struct ::wl_listener * listener, void * data);
	static void _handle_keyboard_grab_end(struct ::wl_listener * listener, void * data);
	static void _handle_touch_grab_begin(struct ::wl_listener * listener, void * data);
	static void _handle_touch_grab_end(struct ::wl_listener * listener, void * data);
	static void _handle_request_set_cursor(struct ::wl_listener * listener, void * data);
	static void _handle_request_set_selection(struct ::wl_listener * listener, void * data);
	static void _handle_set_selection(struct ::wl_listener * listener, void * data);
	static void _handle_request_set_primary_selection(struct ::wl_listener * listener, void * data);
	static void _handle_set_primary_selection(struct ::wl_listener * listener, void * data);
	static void _handle_request_start_drag(struct ::wl_listener * listener, void * data);
	static void _handle_start_drag(struct ::wl_listener * listener, void * data);
};

}
