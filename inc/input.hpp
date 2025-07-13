#pragma once

extern "C" {
#include <wlr/types/wlr_input_device.h>
}

#include "common.hpp"

namespace wlkit {

class Input {
public:
	typedef std::function<void(Input&)> Handler;

private:
	Server * _server;
	struct wlr_input_device * _device;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct wl_listener _destroy_listener;

public:
	Input(
		Server & server,
		struct wlr_input_device * device,
		const Handler & callback);
	~Input();

	[[nodiscard]] Server * server() const;
	[[nodiscard]] struct wlr_input_device * device() const;
	[[nodiscard]] void * data() const;

	Input & set_data(void * data);

	Input & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

}
