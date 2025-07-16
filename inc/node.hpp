#pragma once

#include "common.hpp"

namespace wlkit {

class Node {
public:
	using Handler = std::function<void(Node*)>;
	using ID = uint32_t;

	typedef enum Type {
		ROOT,
		OUTPUT,
		WORKSPACE,
		CONTAINER,
	};

	typedef union {
		Root * root;
		Output * output;
		// Workspace * workspace;
		// Container * container;
	} NodeObject;

private:
	Type _type;
	NodeObject * _object;
	ID _id;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;

	struct ::wl_listener _destroy_listener;

public:
	Node(
		const Type & type,
		NodeObject * object,
		const Handler & callback);
	~Node();

	Node & init();

	[[nodiscard]] Type type() const;
	[[nodiscard]] NodeObject * object() const;
	[[nodiscard]] ID id() const;
	[[nodiscard]] void * data() const;

	Node & set_data(void * data);

	Root & on_destroy(const Handler & handler);

	static struct ::wlr_scene_tree * alloc_scene_tree(struct ::wlr_scene_tree * parent, bool * failed);

private:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
};

}
