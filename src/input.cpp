#include "input.hpp"

using namespace wlkit;

Input::Input(Server * server, const Type & type, struct wlr_input_device * device, const Handler & callback):
_server(server), _type(type), _device(device), _data(nullptr) {
	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Input::~Input() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}
}

bool Input::is_keyboard() const {
	return false;
}

Keyboard * Input::as_keyboard() {
	return nullptr;
}

bool Input::is_pointer() const {
	return false;
}

Pointer * Input::as_pointer() {
	return nullptr;
}

Server * Input::server() const {
	return _server;
}

Input::Type Input::type() const {
	return _type;
}

struct wlr_input_device * Input::device() const {
	return _device;
}

void * Input::data() const {
	return _data;
}

Input & Input::set_data(void *data) {
	_data = data;
	return *this;
}

Input & Input::on_destroy(const Handler &handler) {
	_on_destroy.push_back(handler);
	return *this;
}

void Input::_handle_destroy(struct wl_listener * listener, void * data) {
	Input * input = wl_container_of(listener, input, _destroy_listener);
	delete input;
}
