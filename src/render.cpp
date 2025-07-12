#include "render.hpp"
#include "output.hpp"

using namespace wlkit;

Render::Render(Output & output, struct wlr_buffer_pass_options & pass_opts):
_output(&output), _data(nullptr) {
	if (!_output || !_output->wlr_output() || !&pass_opts) {
		// TODO error
	}

	_state = new wlr_output_state{};
	wlr_output_state_init(_state);

	_pass = wlr_output_begin_render_pass(_output->wlr_output(), _state, &pass_opts);
	if (!_pass) {
		wlr_output_state_finish(_state);
		// TODO error
	}
}

Render::~Render() {
	delete _state;
}

Render & Render::commit() {
	if (!_output || !_output->wlr_output()) {
		return *this;
	}
	auto wlr_output = _output->wlr_output();

	wlr_output_add_software_cursors_to_render_pass(wlr_output, _pass, nullptr);

	if (!wlr_render_pass_submit(_pass)) {
		wlr_output_state_finish(_state);
		// TODO error
	}

	if (!wlr_output_commit_state(wlr_output, _state)) {
		// TODO error
	}

	wlr_output_schedule_frame(wlr_output);
	wlr_output_state_finish(_state);

	return *this;
}

Output * Render::output() const {
	return _output;
}

struct wlr_output_state * Render::state() const {
	return _state;
}

struct wlr_render_pass * Render::pass() const {
	return _pass;
}
