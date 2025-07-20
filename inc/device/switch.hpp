#pragma once

#include <wayland-server-core.h>
extern "C" {
#include <wlr/types/wlr_switch.h>
}

#include "../input.hpp"
#include "pointer.hpp"

namespace wlkit {

class Switch : public Input {
public:
	enum class SwitchType {
		LID,
		TABLET_MODE,
	};

	using ToggleHandler = std::function<
		void(Switch * switch_, SwitchType type, bool state)>;
	using ToggleStateHandler = std::function<
		void(Switch * switch_, SwitchType type)>;

private:
	struct ::wlr_switch * _wlr_switch;

	std::list<ToggleHandler> _on_toggle;
	std::list<ToggleStateHandler> _on_toggle_on;
	std::list<ToggleStateHandler> _on_toggle_off;

	struct ::wl_listener _toggle_listener;

public:
	Switch(
		Server * server,
		struct ::wlr_input_device * device,
		const Handler & callback = nullptr);
	~Switch() override;

	bool is_switch() const override;
	Switch * as_switch() override;

	[[nodiscard]] struct ::wlr_switch * wlr_switch() const;

	Switch & on_toggle(const ToggleHandler & handler);
	Switch & on_toggle_on(const ToggleStateHandler & handler);
	Switch & on_toggle_off(const ToggleStateHandler & handler);

private:
	static void _handle_toggle(struct ::wl_listener * listener, void * data);
};

}
