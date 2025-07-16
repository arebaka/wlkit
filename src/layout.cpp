#include "layout.hpp"

using namespace wlkit;

Layout::Layout(const char * name, const Handler & callback):
_data(nullptr) {
	_name = strdup(name ? name : "");

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
};

Layout::~Layout() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	free(_name);
}

const char * Layout::name() const {
	return _name;
}

void * Layout::data() const {
	return _data;
}

Layout & Layout::set_data(void * data) {
	_data = data;
	return *this;
}

Layout & Layout::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

void Layout::_handle_destroy(struct wl_listener * listener, void * data) {
	Layout * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
