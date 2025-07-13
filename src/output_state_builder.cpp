#include "output.hpp"

using namespace wlkit;

OutputStateBuilder::OutputStateBuilder():
_state{
	true,                          // enabled by default
	1.0f,                          // scale = 1.0
	WL_OUTPUT_TRANSFORM_NORMAL,    // normal orientation
	false,                         // adaptive sync off
	WL_SHM_FORMAT_XRGB8888,        // default render format
	WL_OUTPUT_SUBPIXEL_NONE,       // no subpixel hint
	nullptr,                       // no initial buffer
	nullptr,                       // no initial damage
	nullptr,                       // no layers
	0,                             // layer count = 0
	nullptr,                       // no wait timeline
	0,                             // wait point = 0
	nullptr,                       // no signal timeline
	0                              // signal point = 0
} {}

OutputStateBuilder::~OutputStateBuilder() {}

OutputStateBuilder & OutputStateBuilder::enabled(bool value) {
	_state.enabled = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::scale(Output::Scale value) {
	_state.scale = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::transform(Output::Transform value) {
	_state.transform = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::adaptive_sync_enabled(bool value) {
	_state.adaptive_sync_enabled = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::render_format(Output::RenderFormat value) {
	_state.render_format = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::subpixel(Output::Subpixel value) {
	_state.subpixel = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::buffer(Output::Buffer value) {
	_state.buffer = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::damage(Output::PixmanRegion value) {
	_state.damage = value;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::layers(Output::LayerStates value, size_t n) {
	_state.layers   = value;
	_state.n_layers = n;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::wait(Output::DRMSyncobjTimeline timeline, Output::TimelinePoint src_point) {
	_state.wait_timeline = timeline;
	_state.wait_src_point = src_point;
	return *this;
}

OutputStateBuilder & OutputStateBuilder::signal(Output::DRMSyncobjTimeline timeline, Output::TimelinePoint dst_point) {
	_state.signal_timeline = timeline;
	_state.signal_dst_point = dst_point;
	return *this;
}

std::unique_ptr<Output::State> OutputStateBuilder::build() {
	return std::make_unique<Output::State>(_state);
}
