#pragma once

#include "common.hpp"

namespace wlkit {

class Node {
public:
	typedef std::function<void(Node&)> Handler;
	typedef uint32_t ID;

	typedef enum {
		ROOT,
		OUTPUT,
		WORKSPACE,
		CONTAINER,
	} Type;

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

	struct wl_signal _destroy_event;

public:
	Node(
		const Type & type,
		NodeObject & object);
	~Node();

	Node & init();

	Type type() const;
	NodeObject * object() const;
	ID id() const;
	void * data() const;

	Node & set_data(void * data);
};

}
