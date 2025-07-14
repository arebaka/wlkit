#pragma once

#include "common.hpp"

namespace wlkit {

class Event {
public:
	using Time = uint32_t;

protected:
	const Time _time;

public:
	Event(
		Time time);
	virtual ~Event();

	[[nodiscard]] Time time() const;
};

}
