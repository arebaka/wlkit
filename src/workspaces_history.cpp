#include "workspace.hpp"

using namespace wlkit;

WorkspacesHistory::WorkspacesHistory() {}

WorkspacesHistory::~WorkspacesHistory() {}

const std::list<Workspace*> & WorkspacesHistory::history() const {
	return _order;
}

WorkspacesHistory & WorkspacesHistory::shift(Workspace * workspace) {
	auto it = _pos.find(workspace);
	if (it != _pos.end()) {
		_order.erase(it->second);
	}

	_order.push_front(workspace);
	_pos[workspace] = _order.begin();

	return *this;
}


WorkspacesHistory & WorkspacesHistory::remove(Workspace * workspace) {
	auto it = _pos.find(workspace);
	if (it == _pos.end()) {
		return *this;
	}
	_order.erase(it->second);
	_pos.erase(it);

	return *this;
}

Workspace * WorkspacesHistory::top() const {
	if (_order.size() < 1) {
		return nullptr;
	}
	return *_order.begin();
}

Workspace * WorkspacesHistory::previous() const {
	if (_order.size() < 2) {
		return nullptr;
	}
	return *std::next(_order.begin());
}
