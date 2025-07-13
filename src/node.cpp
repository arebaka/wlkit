#include "../inc/node.hpp"

using namespace wlkit;

Node::Node(const Type & type, NodeObject & object, const Handler & callback):
_type(type), _object(&object), _data(nullptr) {
	if (!_object) {
		// TODO error
	}

	static ID next_id = 0;
	_id = ++next_id;

	_destroy_listener.notify = _handle_destroy;

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(*this);
	}
}

Node::~Node() {
	for (auto & cb : _on_destroy) {
		cb(*this);
	}
}

Node & Node::init() {
	return *this;
}

Node::Type Node::type() const {
	return _type;
}

Node::NodeObject * Node::object() const {
	return _object;
}

Node::ID Node::id() const {
	return _id;
}

void * Node::data() const {
	return _data;
}

Node & Node::set_data(void * data) {
	_data = data;
	return *this;
}

Root & Node::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
}

void Node::_handle_destroy(struct wl_listener * listener, void * data) {
	Node * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
