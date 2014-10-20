#include <pebble.h>

#include "bezier.h"

static Window *window;
static Layer *window_layer;

unsigned short g_prev_hour;
unsigned short g_prev_minute;
unsigned short g_hour;
unsigned short g_minute;
unsigned short changed;

float t_lerp;

static AppTimer *timer;
static const uint32_t timeout_ms = 20;

static void timer_callback(void *context) {
    layer_mark_dirty(window_layer);
    t_lerp += 0.005;

    if(t_lerp < 1) {
        timer = app_timer_register(timeout_ms, timer_callback, NULL);
    } else {
        t_lerp = 0;
        changed = 0;
    }
}

void layer_update(Layer *my_layer, GContext* ctx) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorBlack);

    // Performance: do differential clearing of thing
    graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);

    // Check if any of the digits has changed.
    if(g_prev_hour / 10 != g_hour / 10)
        changed |= (1 << 3);
    if(g_prev_hour % 10 != g_hour % 10)
        changed |= (1 << 2);
    if(g_prev_minute / 10 != g_minute / 10)
        changed |= (1 << 1);
    if(g_prev_minute % 10 != g_minute % 10)
        changed |= 1;

    if(changed && t_lerp == 0)
        timer = app_timer_register(timeout_ms, timer_callback, NULL);


    renderBezierDigit(ctx, g_hour   / 10, t_lerp, 0,  0,  changed & (1 << 3));
    renderBezierDigit(ctx, g_hour   % 10, t_lerp, 70, 0,  changed & (1 << 2));
    renderBezierDigit(ctx, g_minute / 10, t_lerp, 0,  80, changed & (1 << 1));
    renderBezierDigit(ctx, g_minute % 10, t_lerp, 70, 80, changed & 1);

    g_prev_minute = g_minute;
    g_prev_hour = g_hour;
}


void handle_minute_tick(struct tm* t, TimeUnits delta_t) {
    unsigned short hour = (clock_is_24h_style() ? t->tm_hour : (t->tm_hour > 12 ? t->tm_hour - 12 : t->tm_hour));
   
    g_hour = hour;
    g_minute = t->tm_min;

    layer_mark_dirty(window_layer);
}

static void window_load(Window *window) {
    window_layer = window_get_root_layer(window);

    layer_set_update_proc(window_layer, layer_update);
}

static void window_unload(Window *window) {
}

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);

    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
    timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
} 
