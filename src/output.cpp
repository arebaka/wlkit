#include "output.hpp"
#include "server.hpp"
#include "root.hpp"
#include "workspace.hpp"
#include "render.hpp"
#include "window.hpp"

using namespace wlkit;

Output::Output(Server * server, struct wlr_output * wlr_output, const Handler & callback):
_server(server), _wlr_output(wlr_output), _current_workspace(nullptr), _data(nullptr) {
	if (!_server || !_wlr_output) {
		// TODO error
	}

	auto root = _server->root();
	auto allocator = _server->allocator();
	auto renderer = _server->renderer();
	auto event_loop = _server->event_loop();
	if (!root || !root->scene() || !allocator || !renderer || !event_loop) {
		// TODO error
	}

	_scene_output = wlr_scene_output_create(root->scene(), _wlr_output);
	if (!_scene_output) {
		// TODO error
	}

	_repaint_timer = wl_event_loop_add_timer(event_loop, _handle_repaint_timer, this);

	if (!wlr_output_init_render(_wlr_output, allocator, renderer)) {
		wlr_scene_output_destroy(_scene_output);
		// TODO error
	}

	_state = new wlr_output_state{};
	wlr_output_state_init(_state);

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_wlr_output->events.destroy, &_destroy_listener);
	wl_signal_add(&_scene_output->events.destroy, &_destroy_listener);

	_workspaces_history = new WorkspacesHistory();

	struct wlr_output_state state;
	wlr_output_state_init(&state);

	_frame_listener.notify = _handle_frame;
	wl_signal_add(&_wlr_output->events.frame, &_frame_listener);

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Output::~Output() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	delete _workspaces_history;
	delete _state;
	wlr_scene_output_destroy(_scene_output);
}

Output & Output::setup_state(const State * state) {
	wlr_output_state_set_enabled(_state, state->enabled);
	wlr_output_state_set_scale(_state, state->scale);
	wlr_output_state_set_transform(_state, state->transform);
	wlr_output_state_set_adaptive_sync_enabled(_state, state->adaptive_sync_enabled);
	wlr_output_state_set_render_format(_state, state->render_format);
	wlr_output_state_set_subpixel(_state, state->subpixel);
	if (state->buffer != nullptr) {
		wlr_output_state_set_buffer(_state, state->buffer);
	}
	if (state->damage != nullptr) {
		wlr_output_state_set_damage(_state, state->damage);
	}
	if (state->layers != nullptr && state->n_layers > 0) {
		wlr_output_state_set_layers(_state, state->layers, state->n_layers);
	}
	if (state->wait_timeline != nullptr) {
		wlr_output_state_set_wait_timeline(_state, state->wait_timeline, state->wait_src_point);
	}
	if (state->signal_timeline != nullptr) {
		wlr_output_state_set_signal_timeline(_state, state->signal_timeline, state->signal_dst_point);
	}

	return *this;
}

Output & Output::setup_mode(const Mode * mode) {
	wlr_output_state_set_custom_mode(_state, mode->width, mode->height, mode->refresh);
	return *this;
}

Output & Output::setup_preferred_mode() {
	auto mode = wlr_output_preferred_mode(_wlr_output);
	if (mode) {
		wlr_output_state_set_mode(_state, mode);
	}
	return *this;
}

Output & Output::setup_gamma_lut(const GammaLUT * gamma_lut) {
	wlr_output_state_set_gamma_lut(_state, gamma_lut->ramp_size, &gamma_lut->r, &gamma_lut->g, &gamma_lut->b);
	return *this;
}

Output & Output::commit_state() {
	wlr_output_commit_state(_wlr_output, _state);
	wlr_output_state_finish(_state);
	return *this;
}

Window * Output::window_at(Geo x, Geo y) {
	for (Window * window : *_current_workspace->windows_history()) {
		if (window->mapped() &&
			x >= window->x() && x < window->x() + window->width() &&
			y >= window->y() && y < window->y() + window->height()
		) {
			return window;
		}
	}

	return nullptr;
}

struct Server * Output::server() const {
	return _server;
}

struct wlr_output * Output::wlr_output() const {
	return _wlr_output;
}

struct wlr_scene_output * Output::scene_output() const {
	return _scene_output;
}

struct wl_event_source * Output::repaint_timer() const {
	return _repaint_timer;
}

struct timespec Output::last_frame() const {
	return _last_frame;
}

Workspace * Output::current_workspace() const {
	return _current_workspace;
}

void * Output::data() const {
	return _data;
}

std::list<Workspace*> Output::workspaces() const {
	return _workspaces;
}

WorkspacesHistory * Output::workspaces_history() const {
	return _workspaces_history;
}

const char * Output::name() const {
	return _wlr_output->name;
}

const char * Output::description() const {
	return _wlr_output->description;
}

const char * Output::get_make() const {
	return _wlr_output->make;
}

const char * Output::model() const {
	return _wlr_output->model;
}

const char * Output::serial() const {
	return _wlr_output->serial;
}

Geo Output::phys_width() const {
	return _wlr_output->phys_width;
}

Geo Output::phys_height() const {
	return _wlr_output->phys_height;
}

Geo Output::width() const {
	return _wlr_output->width;
}

Geo Output::height() const {
	return _wlr_output->height;
}

Output::ModeRefresh Output::refresh_rate() const {
	return _wlr_output->refresh;
}

bool Output::enabled () const {
	return _wlr_output->enabled;
}

Output::Scale Output::get_scale() const {
	return _wlr_output->scale;
}

wl_output_subpixel Output::subpixel() const {
	return _wlr_output->subpixel;
}

wl_output_transform Output::get_transform() const {
	return _wlr_output->transform;
}

wlr_output_adaptive_sync_status Output::adaptive_sync_status() const {
	return _wlr_output->adaptive_sync_status;
}

Output::RenderFormat Output::render_format() const {
	return _wlr_output->render_format;
}

bool Output::adaptive_sync_supported() const {
	return _wlr_output->adaptive_sync_supported;
}

bool Output::needs_frame() const {
	return _wlr_output->needs_frame;
}

bool Output::frame_pending() const {
	return _wlr_output->frame_pending;
}

bool Output::non_desktop() const {
	return _wlr_output->non_desktop;
}

Output::CommitSeq Output::commit_seq() const {
	return _wlr_output->commit_seq;
}

Output & Output::set_workspace(Workspace * workspace) {
	_current_workspace = workspace;
	_workspaces_history->shift(workspace);
	return *this;
}

Output & Output::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

Output & Output::on_frame(const FrameHandler & handler) {
	if (handler) {
		_on_frame.push_back(std::move(handler));
	}
	return *this;
}

void Output::_handle_destroy(struct wl_listener * listener, void * data) {
	Output * output = wl_container_of(listener, output, _destroy_listener);
	delete output;
}

void Output::_handle_frame(struct wl_listener * listener, void * data) {
	Output * output = wl_container_of(listener, output, _frame_listener);
	auto wlr_output = static_cast<struct wlr_output*>(data);
	if (!wlr_output) {
		return;
	}

	struct wlr_buffer_pass_options pass_opts{};
	auto render = new Render(output, &pass_opts, nullptr);

	for (auto & cb : output->_on_frame) {
		cb(output, wlr_output, render);
	}

	render->commit();
	delete render;
}

int Output::_handle_repaint_timer(void * data) {

}
