#pragma once

extern "C" {
#include <wlr/types/wlr_input_device.h>
}

#include "common.hpp"

namespace wlkit {

class Input {
public:
	using Handler = std::function<void(Input&)>;

	enum class Type {
		KEYBOARD,
		POINTER,
		TOUCH,
		TABLET,
		TABLET_PAD,
		SWITCH
	};

protected:
	Server * _server;
	const Type _type;
	struct ::wlr_input_device * _device;

	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct wl_listener _destroy_listener;

public:
	Input(
		Server * server,
		const Type & type,
		struct ::wlr_input_device * device,
		const Handler & callback);
	virtual ~Input();

	virtual bool is_keyboard() const;
	virtual Keyboard * as_keyboard();
	virtual bool is_pointer() const;
	virtual Pointer * as_pointer();

	[[nodiscard]] Server * server() const;
	[[nodiscard]] Type type() const;
	[[nodiscard]] struct wlr_input_device * device() const;
	[[nodiscard]] void * data() const;

	Input & set_data(void * data);

	Input & on_destroy(const Handler & handler);

protected:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

}
