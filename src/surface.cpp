#include "surface.hpp"
#include "output.hpp"

using namespace wlkit;

Surface::Surface(struct wl_resource * resource, const Handler & callback) {
	if (!resource) {
		// TODO error
	}

	_surface = wlr_surface_from_resource(resource);
	if (!_surface) {
		// TODO error
	}

	if (callback) {
		_on_create.push_back(callback);
		callback(this);
	}
}

Surface::~Surface() {
	for (auto & cb : _on_destroy) {
		cb(this);
	}
}

bool Surface::set_role(struct wlr_surface_role * role, struct wl_resource * error_resource, RoleErrorCode error_code) {
	return wlr_surface_set_role(_surface, role, error_resource, error_code);
}

Surface & Surface::set_role_object(struct wlr_surface_role * role, struct wl_resource * role_resource) {
	if (!wlr_surface_set_role(_surface, role, role_resource, WL_DISPLAY_ERROR_INVALID_OBJECT)) {
		// TODO error
	}
	wlr_surface_set_role_object(_surface, role_resource);

	return *this;
}

Surface & Surface::map() {
	wlr_surface_map(_surface);
	return *this;
}

Surface & Surface::unmap() {
	wlr_surface_unmap(_surface);
	return *this;
}

Surface & Surface::reject_pending(struct wl_resource * resource, PendingCode code, char * message) {
	wlr_surface_reject_pending(_surface, resource, code, "%s", message);
	return *this;
}

bool Surface::has_buffer() {
	return wlr_surface_has_buffer(_surface);
}

struct wlr_texture * Surface::get_texture() {
	return wlr_surface_get_texture(_surface);
}

struct wlr_surface * Surface::get_root_surface() {
	return wlr_surface_get_root_surface(_surface);
}

bool Surface::point_accepts_input(Geo sx, Geo sy, Geo * sub_x, Geo * sub_y) {
	if (sub_x && sub_y) {
		return wlr_surface_surface_at(_surface, sx, sy, sub_x, sub_y) != nullptr;
	} else {
		return wlr_surface_point_accepts_input(_surface, sx, sy);
	}
}

Surface & Surface::send_enter(struct Output * output) {
	wlr_surface_send_enter(_surface, output->wlr_output());
	return *this;
}

Surface & Surface::send_leave(struct Output * output) {
	wlr_surface_send_leave(_surface, output->wlr_output());
	return *this;
}

Surface & Surface::send_frame_done(struct timespec when) {
	wlr_surface_send_frame_done(_surface, &when);
	return *this;
}

Surface & Surface::get_extents(struct wlr_box * box) {
	wlr_surface_get_extents(_surface, box);
	return *this;
}

Surface & Surface::get_effective_damage(pixman_region32_t * damage) {
	wlr_surface_get_effective_damage(_surface, damage);
	return *this;
}

Surface & Surface::get_buffer_source_box(struct wlr_fbox * box) {
	wlr_surface_get_buffer_source_box(_surface, box);
	return *this;
}

Surface::PendingCode Surface::lock_pending() {
	return wlr_surface_lock_pending(_surface);
}

Surface & Surface::unlock_cached(StateSeq seq) {
	wlr_surface_unlock_cached(_surface, seq);
	return *this;
}

Surface & Surface::set_preferred_buffer_scale(int32_t scale) {
	if (scale <= 0) {
		// TODO error
	}
	wlr_surface_set_preferred_buffer_scale(_surface, scale);

	return *this;
}

Surface & Surface::set_preferred_buffer_transform(enum wl_output_transform transform) {
	if (transform < WL_OUTPUT_TRANSFORM_NORMAL || transform > WL_OUTPUT_TRANSFORM_FLIPPED_270) {
		// TODO error
	}
	wlr_surface_set_preferred_buffer_transform(_surface, transform);

	return *this;
}

struct wlr_surface * Surface::wlr_surface() const {
	return _surface;
}

Surface & Surface::on_destroy(const Handler & handler) {
	if (handler) {
		_on_destroy.push_back(handler);
	}
	return *this;
}

Surface & Surface::on_client_commit(const Handler & handler) {
	if (handler) {
		_on_client_commit.push_back(handler);
	}
	return *this;
}

Surface & Surface::on_commit(const Handler & handler) {
	if (handler) {
		_on_commit.push_back(handler);
	}
	return *this;
}

Surface & Surface::on_map(const Handler & handler) {
	if (handler) {
		_on_map.push_back(handler);
	}
	return *this;
}

Surface & Surface::on_unmap(const Handler & handler) {
	if (handler) {
		_on_unmap.push_back(handler);
	}
	return *this;
}

Surface & Surface::on_new_subsurface(const NewSubsurfaceHandler & handler) {
	if (handler) {
		_on_new_subsurface.push_back(handler);
	}
	return *this;
}

void Surface::_handle_destroy(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _destroy_listener);

	for (auto & cb : surface->_on_destroy) {
		cb(surface);
	}

	delete surface;
}

void Surface::_handle_client_commit(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _client_commit_listener);
	for (auto & cb : surface->_on_client_commit) {
		cb(surface);
	}
}

void Surface::_handle_commit(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _commit_listener);
	for (auto & cb : surface->_on_commit) {
		cb(surface);
	}
}

void Surface::_handle_map(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _map_listener);
	for (auto & cb : surface->_on_map) {
		cb(surface);
	}
}

void Surface::_handle_unmap(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _unmap_listener);
	for (auto & cb : surface->_on_unmap) {
		cb(surface);
	}
}

void Surface::_handle_new_subsurface(struct ::wl_listener * listener, void * data) {
	Surface * surface = wl_container_of(listener, surface, _new_subsurface_listener);
	auto * subsurface = static_cast<struct wlr_subsurface*>(data);

	for (auto & cb : surface->_on_new_subsurface) {
		cb(surface, subsurface);
	}
}
