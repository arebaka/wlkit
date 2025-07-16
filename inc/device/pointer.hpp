#pragma once

#include <map>

extern "C" {
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_pointer_gestures_v1.h>
// #include <wlr/types/wlr_pointer_constraints_v1.h>
}

#include "../input.hpp"

namespace wlkit {

class Pointer : public Input {
public:

	using MotionOrientation = int32_t;
	using MotionDelta = double;
	using Button = uint32_t;
	using AxisSource = enum ::wl_pointer_axis_source;
	using AxisOrientation = enum ::wl_pointer_axis;
	using AxisRelativeDirection = enum ::wl_pointer_axis_relative_direction;
	using AxisDelta = double;
	using AxisDeltaDiscrete = int32_t;
	using NFingers = uint32_t;
	using PinchScale = double;
	using PinchRotation = double;

	using MotionHandler = std::function<
		void(Pointer * pointer, MotionDelta dx, MotionDelta dy, MotionDelta unaccel_dx, MotionDelta unaccel_dy)>;
	using ButtonHandler = std::function<
		void(Pointer * pointer, Button button, bool state)>;
	using AxisHandler = std::function<
		void(Pointer * pointer, AxisSource source, AxisOrientation orientation,
			AxisRelativeDirection relative_direction, AxisDelta delta, AxisDeltaDiscrete delta_discrete)>;
	using ActionBeginHandler = std::function<
		void(Pointer * pointer, NFingers n_fingers)>;
	using ActionEndHandler = std::function<
		void(Pointer * pointer, bool cancelled)>;
	using SwipeUpdateHandler = std::function<
		void(Pointer * pointer, NFingers n_fingers, Geo dx, Geo dy)>;
	using PinchUpdateHandler = std::function<
		void(Pointer * pointer, NFingers n_fingers, Geo dx, Geo dy, PinchScale scale, PinchRotation rotation)>;

private:
	struct ::wlr_pointer * _ptr;
	struct ::wlr_pointer_gestures_v1 * _gestures;
	// struct ::wlr_pointer_constraints_v1 * _constraints;
	// std::map<Surface*, struct ::wlr_pointer_constraint_v1*> _constraints_by_surface;

	std::list<Handler> _on_destroy;
	std::list<MotionHandler> _on_motion;
	std::list<ButtonHandler> _on_button;
	std::list<AxisHandler> _on_axis;
	std::list<ActionBeginHandler> _on_swipe_begin;
	std::list<SwipeUpdateHandler> _on_swipe_update;
	std::list<ActionEndHandler> _on_swipe_end;
	std::list<ActionBeginHandler> _on_pinch_begin;
	std::list<PinchUpdateHandler> _on_pinch_update;
	std::list<ActionEndHandler> _on_pinch_end;
	std::list<ActionBeginHandler> _on_hold_begin;
	std::list<ActionEndHandler> _on_hold_end;

	struct ::wl_listener _destroy_listener;
	struct ::wl_listener _motion_listener;
	struct ::wl_listener _button_listener;
	struct ::wl_listener _axis_listener;
	struct ::wl_listener _swipe_begin_listener;
	struct ::wl_listener _swipe_update_listener;
	struct ::wl_listener _swipe_end_listener;
	struct ::wl_listener _pinch_begin_listener;
	struct ::wl_listener _pinch_update_listener;
	struct ::wl_listener _pinch_end_listener;
	struct ::wl_listener _hold_begin_listener;
	struct ::wl_listener _hold_end_listener;

public:
	Pointer(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback);
	~Pointer() override;

	bool is_pointer() const override;
	Pointer * as_pointer() override;

	Pointer & send_swipe_begin(NFingers n_fingers);
	Pointer & send_swipe_update(Geo dx, Geo dy);
	Pointer & send_swipe_end(bool cancelled);
	Pointer & send_pinch_begin(NFingers n_fingers);
	Pointer & send_pinch_update(Geo dx, Geo dy, PinchScale scale, PinchRotation rotation);
	Pointer & send_pinch_end(bool cancelled);
	Pointer & send_hold_begin(NFingers n_fingers);
	Pointer & send_hold_end(bool cancelled);

	Pointer & constraint_for_surface(Surface * surface);
	Pointer & send_constraint_activated(Surface * surface);
	Pointer & send_constraint_deactivated(Surface * surface);

	[[nodiscard]] struct ::wlr_pointer * wlr_pointer() const;

	Pointer & on_destroy(const Handler & handler);
	Pointer & on_motion(const MotionHandler & handler);
	Pointer & on_button(const ButtonHandler & handler);
	Pointer & on_axis(const AxisHandler & handler);
	Pointer & on_swipe_begin(const ActionBeginHandler & handler);
	Pointer & on_swipe_update(const SwipeUpdateHandler & handler);
	Pointer & on_swipe_end(const ActionEndHandler & handler);
	Pointer & on_pinch_begin(const ActionBeginHandler & handler);
	Pointer & on_pinch_update(const PinchUpdateHandler & handler);
	Pointer & on_pinch_end(const ActionEndHandler & handler);
	Pointer & on_hold_begin(const ActionBeginHandler & handler);
	Pointer & on_hold_end(const ActionEndHandler & handler);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
	static void _handle_motion(struct ::wl_listener * listener, void * data);
	static void _handle_button(struct ::wl_listener * listener, void * data);
	static void _handle_axis(struct ::wl_listener * listener, void * data);
	static void _handle_swipe_begin(struct ::wl_listener * listener, void * data);
	static void _handle_swipe_update(struct ::wl_listener * listener, void * data);
	static void _handle_swipe_end(struct ::wl_listener * listener, void * data);
	static void _handle_pinch_begin(struct ::wl_listener * listener, void * data);
	static void _handle_pinch_update(struct ::wl_listener * listener, void * data);
	static void _handle_pinch_end(struct ::wl_listener * listener, void * data);
	static void _handle_hold_begin(struct ::wl_listener * listener, void * data);
	static void _handle_hold_end(struct ::wl_listener * listener, void * data);
};

}
