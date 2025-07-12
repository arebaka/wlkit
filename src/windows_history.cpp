#include "window.hpp"

using namespace wlkit;

WindowsHistory::WindowsHistory() {}

WindowsHistory::~WindowsHistory() {}

const std::list<Window*> & WindowsHistory::history() const {
	return _order;
}

WindowsHistory & WindowsHistory::shift(Window * window) {
	auto it = _pos.find(window);
	if (it != _pos.end()) {
		_order.erase(it->second);
	}

	_order.push_front(window);
	_pos[window] = _order.begin();

	return *this;
}


WindowsHistory & WindowsHistory::remove(Window * window) {
	auto it = _pos.find(window);
	if (it == _pos.end()) {
		return *this;
	}
	_order.erase(it->second);
	_pos.erase(it);

	return *this;
}


Window * WindowsHistory::previous() const {
	if (_order.size() < 2) {
		return nullptr;
	}
	return *std::next(_order.begin());
}
