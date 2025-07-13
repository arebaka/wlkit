#pragma once

#include <ctime>

extern "C" {
#include <wlr/types/wlr_output.h>
}

#include "common.hpp"

namespace wlkit {

class Output {
public:
	typedef std::function<void(Output&)> Handler;

	typedef float Scale;
	typedef enum wl_output_transform Transform;
	typedef uint32_t RenderFormat;
	typedef enum wl_output_subpixel Subpixel;
	typedef struct wlr_buffer* Buffer;
	typedef pixman_region32_t* PixmanRegion;
	typedef struct wlr_output_layer_state* LayerStates;
	typedef struct wlr_drm_syncobj_timeline* DRMSyncobjTimeline;
	typedef uint64_t TimelinePoint;
	typedef uint32_t ModeRefresh;
	typedef size_t GammaLUTRampSize;
	typedef uint16_t GammaLUTComponent;
	typedef uint32_t CommitSeq;

	typedef struct {
		bool enabled;
		Scale scale;
		Transform transform;
		bool adaptive_sync_enabled;
		RenderFormat render_format;
		Subpixel subpixel;
		Buffer buffer;
		PixmanRegion damage;
		LayerStates layers;
		size_t n_layers;
		DRMSyncobjTimeline wait_timeline;
		TimelinePoint wait_src_point;
		DRMSyncobjTimeline signal_timeline;
		TimelinePoint signal_dst_point;
	} State;

	typedef struct {
		Geo width;
		Geo height;
		ModeRefresh refresh;
	} Mode;

	typedef struct {
		GammaLUTRampSize ramp_size;
		GammaLUTComponent r;
		GammaLUTComponent g;
		GammaLUTComponent b;
	} GammaLUT;

private:
	Server * _server;
	struct wlr_output * _wlr_output;

	struct wlr_scene_output * _scene_output;
	struct wl_event_source * _repaint_timer;
	struct wlr_output_state * _state;

	struct timespec _last_frame;
	Workspace * _current_workspace;
	std::list<Workspace*> _workspaces;
	WorkspacesHistory * _workspaces_history;
	void * _data;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	std::list<NotifyHandler> _on_frame;

	struct wl_listener _destroy_listener;
	struct wl_listener _frame_listener;

public:
	Output(
		Server & server,
		struct wlr_output & wlr_output,
		const Handler & callback);
	~Output();

	Output & setup_state(const State * state);
	Output & setup_mode(const Mode * mode);
	Output & setup_preferred_mode();
	Output & setup_gamma_lut(const GammaLUT * gamma_lut);
	Output & commit_state();
	// Output & switch_workspace(Workspace::ID id);
	Window * window_at(Geo x, Geo y);

	[[nodiscard]] Server * server() const;
	[[nodiscard]] struct wlr_output * wlr_output() const;
	[[nodiscard]] struct wlr_scene_output * scene_output() const;
	[[nodiscard]] struct wl_event_source * repaint_timer() const;
	[[nodiscard]] struct wlr_output_state * state() const;
	[[nodiscard]] struct timespec last_frame() const;
	[[nodiscard]] Workspace * current_workspace() const;
	[[nodiscard]] std::list<Workspace*> workspaces() const;
	[[nodiscard]] WorkspacesHistory * workspaces_history() const;
	[[nodiscard]] void * data() const;

	[[nodiscard]] const char * name() const;
	[[nodiscard]] const char * description() const;
	[[nodiscard]] const char * get_make() const;
	[[nodiscard]] const char * model() const;
	[[nodiscard]] const char * serial() const;
	[[nodiscard]] Geo phys_width() const;
	[[nodiscard]] Geo phys_height() const;
	[[nodiscard]] Geo width() const;
	[[nodiscard]] Geo height() const;
	[[nodiscard]] ModeRefresh refresh_rate() const;
	[[nodiscard]] bool enabled () const;
	[[nodiscard]] Scale get_scale() const;
	[[nodiscard]] wl_output_subpixel subpixel() const;
	[[nodiscard]] wl_output_transform get_transform() const;
	[[nodiscard]] wlr_output_adaptive_sync_status adaptive_sync_status() const;
	[[nodiscard]] RenderFormat render_format() const;
	[[nodiscard]] bool adaptive_sync_supported() const;
	[[nodiscard]] bool needs_frame() const;
	[[nodiscard]] bool frame_pending() const;
	[[nodiscard]] bool non_desktop() const;
	[[nodiscard]] CommitSeq commit_seq() const;

	// TODO setters

	Output & on_destroy(const Handler & handler);
	Output & on_frame(const NotifyHandler & handler);

private:
	static void _handle_destroy(struct wl_listener * listener, void * data);
	static void _handle_frame(struct wl_listener * listener, void * data);
	static int _handle_repaint_timer(void * data);
};

class OutputStateBuilder {
private:
	Output::State _state;

public:
	OutputStateBuilder();
	~OutputStateBuilder();

	OutputStateBuilder & enabled(bool value);
	OutputStateBuilder & scale(Output::Scale value);
	OutputStateBuilder & transform(Output::Transform value);
	OutputStateBuilder & adaptive_sync_enabled(bool value);
	OutputStateBuilder & render_format(Output::RenderFormat value);
	OutputStateBuilder & subpixel(Output::Subpixel value);
	OutputStateBuilder & buffer(Output::Buffer value);
	OutputStateBuilder & damage(Output::PixmanRegion value);
	OutputStateBuilder & layers(Output::LayerStates v, size_t n);
	OutputStateBuilder & wait(Output::DRMSyncobjTimeline timeline, Output::TimelinePoint src_point);
	OutputStateBuilder & signal(Output::DRMSyncobjTimeline timeline, Output::TimelinePoint dst_point);

	std::unique_ptr<Output::State> build();
};

}
