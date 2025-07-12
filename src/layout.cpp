#include "layout.hpp"

using namespace wlkit;

Layout::Layout(const char * name) {
	_name = strdup(name ? name : "");
};

Layout::~Layout() {
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
