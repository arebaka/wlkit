#ifndef WLCOM_H
#define WLCOM_H

#include <stdint.h>
#include <sys/types.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>
#include <wlr/util/log.h>
#include <wlr/xwayland.h>
#include <xkbcommon/xkbcommon.h>

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

struct wlcom_server;
struct wlcom_output;
struct wlcom_workspace;
struct wlcom_layout;
struct wlcom_window;

struct wlcom_focus_manager;
struct wlcom_keyboard_manager;
struct wlcom_portal_manager;

typedef uint32_t wlcom_workspace_id;
typedef uint32_t wlcom_screen_request_id;

struct wlcom_server {
    struct wl_display * wl_display;
    struct wl_event_loop * wl_event_loop;

    struct wlr_backend * backend;
    struct wlr_renderer * renderer;

    struct wlr_compositor * compositor;
    struct wlr_xdg_shell * xdg_shell;
    struct wlr_data_device_manager * data_device_manager;
    struct wlr_seat * seat;
    struct wlr_output_layout * output_layout;

    struct wlcom_workspace * current_workspace;

    struct wl_list outputs;
    struct wl_list inputs;
    struct wl_list workspaces;
    struct wl_list windows;

    struct wlcom_focus_manager * focus_manager;
    struct wlcom_keyboard_manager * keyboard_manager;
    struct wlcom_portal_manager * portal_manager;

    struct wl_listener new_output;
    struct wl_listener new_input;
    struct wl_listener new_xdg_surface;

    const char * socket;
	bool running;

	uint8_t _reserved[255];
    void * user_data;
};

struct wlcom_output {
	struct wlcom_server * server;
    struct wlr_output * wlr_output;

    struct timespec last_frame;
    struct wl_list link;

    struct wl_listener frame;
    struct wl_listener destroy;

	uint8_t _reserved[24];
    void * user_data;
};

struct wlcom_input {
    struct wl_list link;

    struct wlcom_server * server;
    struct wlr_input_device * device;

    struct wl_listener destroy;

	uint8_t _reserved[64];
    void * user_data;
};

struct wlcom_workspace {
	struct wl_list link;

    wlcom_workspace_id id;
    char * name;

    struct wl_list windows;

    struct wlcom_server * server;
    struct wlcom_layout * layout;
    struct wlcom_window * focused_window;

	uint8_t _reserved[48];
    void * user_data;
};

struct wlcom_layout {
    char * name;

    void (*arrange)(struct wlcom_workspace * workspace);
    void (*handle_new_window)(struct wlcom_workspace * workspace, struct wlcom_window * window);
    void (*handle_window_removed)(struct wlcom_workspace * workspace, struct wlcom_window * window);
    void (*handle_window_focus)(struct wlcom_workspace * workspace, struct wlcom_window * window);

	uint8_t _reserved[16];
    void * user_data;
};

struct wlcom_window {
    struct wl_list link;

    char * title;
    char * app_id;

    int x, y, width, height;
    bool mapped, minimized, maximized, fullscreen;
	uint8_t _reserved_flags[4];

    struct wlcom_server * server;
	struct wlcom_workspace * workspace;

    struct wlr_surface * surface;
    struct wlr_xdg_surface * xdg_surface;
	struct wlr_xwayland_surface * xwayland_surface;
    struct wlr_foreign_toplevel_handle_v1 * foreign_toplevel;

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener destroy;
    struct wl_listener set_title;
    struct wl_listener set_app_id;

    struct wl_listener xwayland_map;
    struct wl_listener xwayland_unmap;
    struct wl_listener xwayland_destroy;
    struct wl_listener xwayland_set_title;
    struct wl_listener xwayland_set_class;

	struct wl_listener request_activate;
	struct wl_listener request_close;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
	struct wl_listener request_minimize;
    struct wl_listener request_fullscreen;

	uint8_t _reserved[64];
    void * user_data;
};

struct wlcom_focus_manager {
    struct wlcom_server * server;
    struct wlcom_window * focused_window;
    struct wl_list focus_stack;
};

struct wlcom_keyboard_layout {
    struct wl_list link;
	char * rules;
	char * model;
    char * name;
    char * variant;
    char * options;
	uint8_t _reserved[8];
};

struct wlcom_keyboard_manager {
    struct wlcom_server * server;
    struct wlcom_keyboard_layout * current_layout;
    struct wl_list layouts;
};

struct wlcom_portal_manager {
    struct wlcom_server * server;

    struct wlr_xdg_output_manager_v1 * xdg_output_manager;
    struct wlr_screencopy_manager_v1 * screencopy_manager;
    struct wlr_export_dmabuf_manager_v1 * export_dmabuf_manager;
    struct wlr_gamma_control_manager_v1 * gamma_manager;
    struct wlr_foreign_toplevel_manager_v1 * foreign_toplevel_manager;

    struct wl_list screenshot_requests;
    struct wl_list screencast_sessions;

    wlcom_screen_request_id next_screen_request_id;
	uint8_t _reserved[44];
};

enum wlcom_screenshot_type {
    WLCOM_SCREENSHOT_MONITOR,
    WLCOM_SCREENSHOT_WINDOW,
    WLCOM_SCREENSHOT_REGION,
};

struct wlcom_screenshot_request {
    struct wl_list link;

    wlcom_screen_request_id id;
    enum wlcom_screenshot_type type;
	bool interactive;
	uint8_t _reserved[7];

    struct wlr_output * output;
    struct wlcom_window * window;
    struct wlr_box * region;

    void * user_data;
};

struct wlcom_screencast_session {
    struct wl_list link;

    wlcom_screen_request_id id;
    bool active;
	uint8_t _reserved[15];

    struct wlr_output * output;
    struct wlcom_window * window;

    void * user_data;
};

struct wlcom_window_callbacks {
    void (*on_new_window)(struct wlcom_window * window);
    void (*on_window_destroy)(struct wlcom_window * window);
    void (*on_window_focus)(struct wlcom_window * window);
    void (*on_window_unfocus)(struct wlcom_window * window);
    void (*on_window_move)(struct wlcom_window * window, int x, int y);
    void (*on_window_resize)(struct wlcom_window * window, int width, int height);
    void (*on_window_maximize)(struct wlcom_window * window);
    void (*on_window_minimize)(struct wlcom_window * window);
    void (*on_window_fullscreen)(struct wlcom_window * window);
    void (*on_workspace_changed)(wlcom_workspace_id old_workspace, wlcom_workspace_id new_workspace);
};

struct wlcom_portal_callbacks {
    void (*on_screenshot_request)(struct wlcom_screenshot_request * request);
    void (*on_screenshot_confirmed)(struct wlcom_screenshot_request * request);
    void (*on_screenshot_cancelled)(struct wlcom_screenshot_request * request);
    void (*on_screencast_start)(struct wlcom_screencast_session * session);
    void (*on_screencast_stop)(struct wlcom_screencast_session * session);

    struct wlcom_window * (*select_window)();
    struct wlr_output * (*select_output)();
    struct wlr_box * (*select_region)();
};

struct wlcom_keyboard_callbacks {
    void (*on_layout_changed)(struct wlcom_keyboard_layout * layout);
    void (*on_key_pressed)(struct wlr_event_keyboard_key * event);
    void (*on_key_released)(struct wlr_event_keyboard_key * event);
};

// Main API functions
struct wlcom_server * wlcom_create(struct wl_display * display, struct wlr_seat * seat);
void wlcom_destroy(struct wlcom_server * server);
bool wlcom_start(struct wlcom_server * server);
void wlcom_run(struct wlcom_server * server);
void wlcom_stop(struct wlcom_server * server);

// Workspace management
struct wlcom_workspace * wlcom_workspace_create(struct wlcom_server * server, struct wlcom_layout * layout, wlcom_workspace_id id, const char * name);
void wlcom_workspace_destroy(struct wlcom_workspace * workspace, struct wlcom_workspace * reserved_workspace);
void wlcom_workspace_switch(struct wlcom_server * server, wlcom_workspace_id workspace_id);
struct wlcom_workspace * wlcom_workspace_get(struct wlcom_server * server, wlcom_workspace_id workspace_id);

// Layout management
void wlcom_layout_register(struct wlcom_layout * layout);
void wlcom_workspace_set_layout(struct wlcom_workspace * workspace, struct wlcom_layout * layout);
void wlcom_layout_arrange(struct wlcom_workspace * workspace);

// Window management
void wlcom_window_move(struct wlcom_window * window, int x, int y);
void wlcom_window_resize(struct wlcom_window * window, int width, int height);
void wlcom_window_close(struct wlcom_window * window);
void wlcom_window_maximize(struct wlcom_window * window);
void wlcom_window_minimize(struct wlcom_window * window);
void wlcom_window_fullscreen(struct wlcom_window * window);
void wlcom_window_set_workspace(struct wlcom_window * window, wlcom_workspace_id workspace_id);

// Focus management
void wlcom_focus_window(struct wlcom_window * window);
struct wlcom_window * wlcom_get_focused_window(struct wlcom_server * server);
void wlcom_focus_next_window(struct wlcom_server * server);
void wlcom_focus_prev_window(struct wlcom_server * server);

// Keyboard management
void wlcom_keyboard_add_layout(struct wlcom_keyboard_manager * manager, const char * rules, const char * model, const char * name, const char * variant, const char * options);
void wlcom_keyboard_switch_layout(struct wlcom_keyboard_manager * manager, const char * name);
void wlcom_keyboard_next_layout(struct wlcom_keyboard_manager * manager);

// Portal management
struct wlcom_portal_manager * wlcom_portal_create(struct wlcom_server * server);
void wlcom_portal_destroy(struct wlcom_portal_manager * manager);
void wlcom_portal_setup_protocols(struct wlcom_portal_manager * manager);

// Portal screenshot/screencast
void wlcom_portal_handle_screenshot(struct wlcom_portal_manager * manager, struct wlcom_screenshot_request * request);
void wlcom_portal_start_screencast(struct wlcom_portal_manager * manager, struct wlcom_screencast_session * session);
void wlcom_portal_stop_screencast(struct wlcom_portal_manager * manager, struct wlcom_screencast_session * session);

// Callback registration
void wlcom_set_window_callbacks(struct wlcom_server * server, struct wlcom_window_callbacks * callbacks);
void wlcom_set_portal_callbacks(struct wlcom_portal_manager * manager, struct wlcom_portal_callbacks * callbacks);
void wlcom_set_keyboard_callbacks(struct wlcom_keyboard_manager * manager, struct wlcom_keyboard_callbacks * callbacks);

// Built-in layouts
extern struct wlcom_layout wlcom_layout_tiling;
extern struct wlcom_layout wlcom_layout_floating;
extern struct wlcom_layout wlcom_layout_fullscreen;

// Utility functions
struct wlcom_window * wlcom_window_at(struct wlcom_server * server, double x, double y);
void wlcom_setup_portal_env(struct wlcom_server * server);

#endif // WLCOM_H
