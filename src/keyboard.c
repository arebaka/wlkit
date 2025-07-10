#include "wlkit.h"

#include <stdlib.h>
#include <string.h>
#include <xkbcommon/xkbcommon.h>

#include <wlr/types/wlr_seat.h>

static struct wlkit_keyboard_callbacks * keyboard_callbacks = NULL;

void wlkit_keyboard_layout_destroy(struct wlkit_keyboard_layout * layout) {
	free(layout->rules);
	free(layout->model);
	free(layout->name);
	free(layout->variant);
	free(layout->options);
	free(layout);
}

void wlkit_keyboard_add_layout(
	struct wlkit_keyboard_manager * manager,
	const char * rules,
	const char * model,
	const char * name,
	const char * variant,
	const char * options
) {
	if (!manager || !name) {
		return;
	}

	struct wlkit_keyboard_layout * layout = malloc(sizeof(*layout));
	layout->rules = strdup(rules);
	layout->model = strdup(model);
	layout->name = strdup(name);
	layout->variant = variant ? strdup(variant) : NULL;
	layout->options = options ? strdup(options) : NULL;

	wl_list_insert(&manager->layouts, &layout->link);

	// Set as current if it's the first layout
	if (!manager->current_layout) {
		manager->current_layout = layout;
	}
}

void wlkit_keyboard_switch_layout(struct wlkit_keyboard_manager * manager, const char * name) {
	if (!manager || !name) {
		return;
	}

	struct wlkit_keyboard_layout * layout;
	wl_list_for_each(layout, &manager->layouts, link) {
		if (strcmp(layout->name, name) == 0) {
			manager->current_layout = layout;

			// Update XKB keymap
			struct xkb_context * context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
			struct xkb_keymap * keymap = xkb_keymap_new_from_names(context,
				&(struct xkb_rule_names){
					.rules = layout->rules,
					.model = layout->model,
					.layout = layout->name,
					.variant = layout->variant,
					.options = layout->options,
				}, XKB_KEYMAP_COMPILE_NO_FLAGS);

			if (keymap) {
				wlr_seat_set_keyboard(manager->server->seat,
					manager->server->seat->keyboard_state.keyboard);
				wlr_keyboard_set_keymap(manager->server->seat->keyboard_state.keyboard, keymap);

				if (keyboard_callbacks && keyboard_callbacks->on_layout_changed) {
					keyboard_callbacks->on_layout_changed(layout);
				}
			}

			xkb_keymap_unref(keymap);
			xkb_context_unref(context);
			break;
		}
	}
}

void wlkit_keyboard_next_layout(struct wlkit_keyboard_manager * manager) {
	if (!manager || !manager->current_layout) {
		return;
	}

	struct wlkit_keyboard_layout * current = manager->current_layout;
	struct wlkit_keyboard_layout * next = NULL;

	if (current->link.next != &manager->layouts) {
		next = wl_container_of(current->link.next, next, link);
	} else {
		// Wrap to first layout
		next = wl_container_of(manager->layouts.next, next, link);
	}

	if (next) {
		wlkit_keyboard_switch_layout(manager, next->name);
	}
}

void wlkit_set_keyboard_callbacks(struct wlkit_keyboard_manager * manager, struct wlkit_keyboard_callbacks * callbacks) {
	(void) manager; // unused for now
	keyboard_callbacks = callbacks;
}
