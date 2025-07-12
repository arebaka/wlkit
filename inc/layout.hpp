#pragma once

#include "common.hpp"

namespace wlkit {

class Layout {
public:
	typedef std::function<void(Layout&)> Handler;

private:
	char * _name;
	void * _data;

public:
	Layout(
		const char * name);
	~Layout();

	// Layout & arrange(Workspace * workspace);
	// Layout & handle_new_window(Workspace * workspace, Window * window);
	// Layout & handle_window_removed(Workspace * workspace, Window * window);
	// Layout & handle_window_focus(Workspace * workspace, Window * window);

	const char * name() const;
	void * data() const;

	Layout & set_data(void * data);
};

}
