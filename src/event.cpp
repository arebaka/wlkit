#include "event.hpp"

using namespace wlkit;

Event::Event(Time time):
_time(time) {}

Event::~Event() {}

Event::Time Event::time() const {
	return _time;
}
