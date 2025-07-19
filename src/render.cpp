#include "render.hpp"
#include "output.hpp"

using namespace wlkit;

Render::Render(Output * output, struct wlr_buffer_pass_options * pass_opts, const Handler & callback):
_output(output), _data(nullptr) {
	if (!_output || !_output->wlr_output() || !pass_opts) {
		// TODO error
	}

	_state = new wlr_output_state{};
	wlr_output_state_init(_state);

	pixman_region32_t buffer_damage;
	pixman_region32_init(&buffer_damage);

	// bool needs_frame;
	// if (!wlr_output_damage_attach_render(output->damage, &needs_frame, &buffer_damage)) {
	// 	wlr_output_state_finish(_state);
	// 	pixman_region32_fini(&buffer_damage);
	// 	free(_state);
	// 	// TODO error
	// }

	// if (!needs_frame) {
	//	wlr_output_rollback(output->wlr_output);
	// 	wlr_output_state_finish(_state);
	// 	pixman_region32_fini(&buffer_damage);
	// 	free(_state);
	// }

	_pass = wlr_output_begin_render_pass(_output->wlr_output(), _state, pass_opts);
	if (!_pass) {
		wlr_output_state_finish(_state);
		pixman_region32_fini(&buffer_damage);
		free(_state);
		// TODO error
	}

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Render::~Render() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

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

Render & Render::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

void Render::_handle_destroy(struct wl_listener * listener, void * data) {
	Render * render = wl_container_of(listener, render, _destroy_listener);
	delete render;
}
