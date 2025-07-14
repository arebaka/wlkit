#pragma once

extern "C" {
#include <wlroots-0.19/wlr/types/wlr_pointer.h>
}

#include "input.hpp"

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

	using MotionHandler = std::function<
		void(Pointer * pointer, MotionDelta dx, MotionDelta dy, MotionDelta unaccel_dx, MotionDelta unaccel_dy)>;
	using ButtonHandler = std::function<
		void(Pointer * pointer, Button button, bool state)>;
	using AxisHandler = std::function<
		void(Pointer * pointer, AxisSource source, AxisOrientation orientation,
			AxisRelativeDirection relative_direction, AxisDelta delta, AxisDeltaDiscrete delta_discrete)>;

private:
	struct ::wlr_pointer * _ptr;

	std::list<MotionHandler> _on_motion;
	std::list<ButtonHandler> _on_button;
	std::list<AxisHandler> _on_axis;

	wl_listener _motion_listener;
	wl_listener _button_listener;
	wl_listener _axis_listener;

public:
	Pointer(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback);
	~Pointer() override;

	bool is_pointer() const override;
	Pointer * as_pointer() override;

	Pointer & on_motion(const MotionHandler & handler);
	Pointer & on_button(const ButtonHandler & handler);
	Pointer & on_axis(const AxisHandler & handler);

private:
	static void _handle_motion(struct wl_listener * listener, void * data);
	static void _handle_button(struct wl_listener * listener, void * data);
	static void _handle_axis(struct wl_listener * listener, void * data);
};

}
