#include "wlkit.h"

void wlkit_assume_notify_handler(struct wl_list * handlers, wlkit_notify_handler_t handler) {
	struct wlkit_notify_handler * wrapper = malloc(sizeof(*wrapper));
	wrapper->handler = handler;
	wl_list_insert(handlers, &wrapper->link);
}
