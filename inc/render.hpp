#pragma once

extern "C" {
#include <wlr/types/wlr_output.h>
#include <wlr/render/pass.h>
}

#include "common.hpp"

namespace wlkit {

class Render {
public:
	using Handler = std::function<void(Render*)>;

private:
	Output * _output;

	struct ::wlr_output_state * _state;
	struct ::wlr_render_pass * _pass;

	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	wl_listener _destroy_listener;

public:
	Render(
		Output * output,
		struct wlr_buffer_pass_options * pass_opts,
		const Handler & callback);
	~Render();

	Render & commit();

	[[nodiscard]] Output * output() const;
	[[nodiscard]] struct wlr_output_state * state() const;
	[[nodiscard]] struct wlr_render_pass * pass() const;

	// TODO setters

	Render & on_destroy(const Handler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
};

}
