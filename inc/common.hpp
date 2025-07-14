#pragma once
#ifndef WLKIT_COMMON_H
#define WLKIT_COMMON_H

#include <cstdint>
#include <variant>
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

using Geo = uint32_t;

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

using Object = std::variant<
	Server*,
	Root*,
	Node*,
	Cursor*,
	Output*,
	Render*,
	Workspace*,
	Window*,
	Input*,
	Keyboard*
>;

}

#endif // WLKIT_COMMON_H
