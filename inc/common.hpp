#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <memory>

extern "C" {
#include <wayland-server-core.h>
#include <wayland-util.h>

// #include <wlr/util/addon.h>
// #include <wlr/util/box.h>
// #include <wlr/util/edges.h>
#include <wlr/util/log.h>
// #include <wlr/util/region.h>
// #include <wlr/util/transform.h>

#define static
#include <wlr/types/wlr_scene.h>
#undef static
// #define class class_
// #include <wlr/xwayland/xwayland.h>
// #undef class
}

namespace wlkit {

typedef uint32_t Geo;

class Server;
class Root;
class Node;
class Cursor;
class Output;
class OutputStateBuilder;
class Render;
class Workspace;
class WorkspacesHistory;
class Layout;
class Window;
class WindowsHistory;
class Input;
class Keyboard;

union Object {
	Server * server;
	Root * root;
	Node * node;
	Cursor * cursor;
	Output * output;
	Render * render;
	Workspace * workspace;
	Window * window;
	Input * input;
	Keyboard * keyboard;
};

using NotifyHandler = std::function<void(struct wl_listener * listener, void * data, Object & object)>;

}
