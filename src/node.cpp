#include "../inc/node.hpp"

using namespace wlkit;

Node::Node(const Type & type, NodeObject & object):
_type(type), _object(&object), _data(nullptr) {
	if (!_object) {
		// TODO error
	}

	static ID next_id = 0;
	_id = ++next_id;
}

Node::~Node() {}

Node & Node::init() {
	wl_signal_init(&_destroy_event);
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
