#include "cursor.hpp"
#include "root.hpp"

using namespace wlkit;

Cursor::Cursor(Root * root, char * name, const Size & size, const Handler & callback):
_root(root), _data(nullptr) {
	if (!_root || !_root->output_layout()) {
		// TODO error
	}

	_wlr_cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(_wlr_cursor, _root->output_layout());

	_xcursor_manager = wlr_xcursor_manager_create(name, size);
	wlr_xcursor_manager_load(_xcursor_manager, 1);

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(this);
	}
}

Cursor::~Cursor() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}

	wlr_cursor_destroy(_wlr_cursor);
	wlr_xcursor_manager_destroy(_xcursor_manager);
}

Root * Cursor::root() const {
	return _root;
}

struct wlr_cursor * Cursor::wlr_cursor() const {
	return _wlr_cursor;
}

struct wlr_xcursor_manager * Cursor::wlr_xcursor_manager() const {
	return _xcursor_manager;
}

void * Cursor::data() const {
	return _data;
}

Cursor & Cursor::set_data(void * data) {
	_data = data;
	return *this;
}

Cursor & Cursor::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
	return *this;
}

void Cursor::_handle_destroy(struct wl_listener * listener, void * data) {
	Cursor * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
