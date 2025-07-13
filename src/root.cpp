#include "root.hpp"
#include "node.hpp"
#include "server.hpp"
#include "node.hpp"
#include "cursor.hpp"

using namespace wlkit;

static struct wlr_scene_tree * alloc_scene_tree(struct wlr_scene_tree * parent, bool * failed) {
	if (*failed) {
		return nullptr;
	}

	struct wlr_scene_tree * tree = wlr_scene_tree_create(parent);
	if (!tree) {
		*failed = true;
	}

	return tree;
}

Root::Root(Server * server, char * cursor_name, const Cursor::Size & cursor_size, const Handler & callback):
_server(server), _x(0), _y(0), _width(0), _height(0), _data(nullptr) {
	if (!_server || !_server->display()) {
		// TODO error
	}

	_scene = wlr_scene_create();
	if (!_scene) {
		// TODO error
	}

	_output_layout = wlr_output_layout_create(_server->display());
	if (!_output_layout) {
		// TODO error
	}

	bool failed = false;
	_staging = alloc_scene_tree(&_scene->tree, &failed);
	_layer_tree = alloc_scene_tree(&_scene->tree, &failed);
	if (failed) {
		// TODO error
	}

	_destroy_listener.notify = _handle_destroy;
	wl_signal_add(&_output_layout->events.destroy, &_destroy_listener);

	Node::NodeObject object{ .root = this };
	_node = new Node(Node::ROOT, &object, nullptr);
	_node->init();

	_cursor = new Cursor(this, cursor_name, cursor_size, nullptr);

	if (callback) {
		_on_create.push_back(std::move(callback));
		callback(*this);
	}
}

Root::~Root() {
	for (auto & cb : _on_destroy) {
		cb(*this);
	}

	delete _cursor;
	delete _node;
	wlr_output_layout_destroy(_output_layout);
	wlr_scene_node_destroy(&_scene->tree.node);
}

struct wlr_scene * Root::scene() const {
	return _scene;
}

struct wlr_output_layout * Root::output_layout() const {
	return _output_layout;
}

Root & Root::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(std::move(handler));
	}
}

void Root::_handle_destroy(struct wl_listener * listener, void * data) {
	Root * it = wl_container_of(listener, it, _destroy_listener);
	delete it;
}
