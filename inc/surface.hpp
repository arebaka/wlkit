#pragma once

extern "C" {
#include <wlr/types/wlr_subcompositor.h>
}

#include "common.hpp"

namespace wlkit {

class Surface {
public:
	using RoleErrorCode = uint32_t;
	using PendingCode = uint32_t;
	using StateSeq = uint32_t;
	using BufferScale = int32_t;
	using Serial = uint32_t;

	using Handler = std::function<void(Surface*)>;
	using NewSubsurfaceHandler = std::function<
		void(Surface * surface, struct ::wlr_subsurface * subsurface)>;

protected:
	struct ::wlr_surface * _surface;

	std::list<Handler> _on_create;
	std::list<Handler> _on_destroy;
	std::list<Handler> _on_client_commit;
	std::list<Handler> _on_commit;
	std::list<Handler> _on_map;
	std::list<Handler> _on_unmap;
	std::list<NewSubsurfaceHandler> _on_new_subsurface;

	struct ::wl_listener _destroy_listener;
	struct ::wl_listener _client_commit_listener;
	struct ::wl_listener _commit_listener;
	struct ::wl_listener _map_listener;
	struct ::wl_listener _unmap_listener;
	struct ::wl_listener _new_subsurface_listener;

public:
	Surface(
		struct ::wlr_surface * surface,
		const Handler & callback = nullptr);
	Surface(
		struct ::wl_resource * resource,
		const Handler & callback = nullptr);
	virtual ~Surface();

	virtual bool is_xdg_toplevel() const;
	virtual XDGToplevel * as_xdg_toplevel();
	virtual bool is_xdg_popup() const;
	virtual XDGPopup * as_xdg_popup();
	virtual bool is_xwayland() const;
	virtual XWayland * as_xwayland();

	virtual bool initialized() const;
	virtual const char * title() const;
	virtual const char * app_id() const;
	virtual void ping();
	virtual void close();
	virtual Serial set_size(Geo width, Geo height);
	virtual Serial set_maximized(bool maximized);
	virtual Serial set_fullscreen(bool fullscreen);
	virtual Serial set_activated(bool activated);

	bool set_role(struct ::wlr_surface_role * role, struct ::wl_resource * error_resource, RoleErrorCode error_code);
	Surface & set_role_object(struct ::wlr_surface_role * role, struct ::wl_resource * role_resource);
	Surface & map();
	Surface & unmap();
	Surface & reject_pending(struct ::wl_resource * resource, PendingCode code, char * message);
	bool has_buffer();
	struct ::wlr_texture * get_texture();
	struct ::wlr_surface * get_root_surface();
	bool point_accepts_input(Geo sx, Geo sy, Geo * sub_x, Geo * sub_y);
	Surface & send_enter(struct Output * output);
	Surface & send_leave(struct Output * output);
	Surface & send_frame_done(struct timespec when);
	Surface & get_extents(struct ::wlr_box * box);
	Surface & get_effective_damage(pixman_region32_t * damage);
	Surface & get_buffer_source_box(struct ::wlr_fbox * box);
	PendingCode lock_pending();
	Surface & unlock_cached(StateSeq seq);
	Surface & set_preferred_buffer_scale(BufferScale scale);
	Surface & set_preferred_buffer_transform(enum ::wl_output_transform transform);

	[[nodiscard]] struct ::wlr_surface * wlr_surface() const;

	Surface & on_destroy(const Handler & handler);
	Surface & on_client_commit(const Handler & handler);
	Surface & on_commit(const Handler & handler);
	Surface & on_map(const Handler & handler);
	Surface & on_unmap(const Handler & handler);
	Surface & on_new_subsurface(const NewSubsurfaceHandler & handler);

protected:
	static void _handle_destroy(struct ::wl_listener * listener, void * data);
	static void _handle_client_commit(struct ::wl_listener * listener, void * data);
	static void _handle_commit(struct ::wl_listener * listener, void * data);
	static void _handle_map(struct ::wl_listener * listener, void * data);
	static void _handle_unmap(struct ::wl_listener * listener, void * data);
	static void _handle_new_subsurface(struct ::wl_listener * listener, void * data);
};

}
