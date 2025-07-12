#pragma once

extern "C" {
#include <wlr/types/wlr_output.h>
#include <wlr/render/pass.h>
}

#include "common.hpp"

namespace wlkit {

class Render {
public:
	typedef std::function<void(Render&)> Handler;

private:
	Output * _output;

	struct wlr_output_state * _state;
	struct wlr_render_pass * _pass;

	void * _data;

public:
	Render(
		Output & output,
		struct wlr_buffer_pass_options & pass_opts);
	~Render();

	Render & commit();

	Output * output() const;
	struct wlr_output_state * state() const;
	struct wlr_render_pass * pass() const;
};

}
