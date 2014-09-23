#include <pebble.h>
 
Window* g_window;
TextLayer *g_text_layer;

static GFont s_res_roboto_bold_subset_49;
static TextLayer *s_textlayer_hour;
static TextLayer *s_textlayer_minute;
 
void window_load(Window *window)
{
	s_res_roboto_bold_subset_49 = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
	// s_textlayer_hour
	s_textlayer_hour = text_layer_create(GRect(15, 15, 115, 65));
	text_layer_set_text(s_textlayer_hour, "12");
	text_layer_set_text_alignment(s_textlayer_hour, GTextAlignmentCenter);
	text_layer_set_font(s_textlayer_hour, s_res_roboto_bold_subset_49);
	layer_add_child(window_get_root_layer(window), (Layer *)s_textlayer_hour);

	// s_textlayer_minute
	s_textlayer_minute = text_layer_create(GRect(15, 85, 115, 65));
	text_layer_set_text(s_textlayer_minute, "59");
	text_layer_set_text_alignment(s_textlayer_minute, GTextAlignmentCenter);
	text_layer_set_font(s_textlayer_minute, s_res_roboto_bold_subset_49);
	layer_add_child(window_get_root_layer(window), (Layer *)s_textlayer_minute);	
}
 
void window_unload(Window *window)
{
	//We will safely destroy the Window's elements here!
	text_layer_destroy(g_text_layer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	//Allocate long-lived storage (required by TextLayer)
	static char buffer_h[] = "00";
	static char buffer_m[] = "00";

	//Write the time to the buffer in a safe manner
	strftime(buffer_h, sizeof("00"), "%H", tick_time); 
	strftime(buffer_m, sizeof("00"), "%M", tick_time);

	//Set the TextLayers to display the buffer
	text_layer_set_text(s_textlayer_hour, buffer_h);
	text_layer_set_text(s_textlayer_minute, buffer_m);
}

void bluetooth_handler(bool connected)
{
	//invert colors
	if (connected){
		// set bg black and text white
		window_set_background_color(g_window, GColorBlack);
		text_layer_set_background_color(s_textlayer_hour, GColorBlack);	
		text_layer_set_background_color(s_textlayer_minute, GColorBlack);
		text_layer_set_text_color(s_textlayer_hour, GColorWhite);
		text_layer_set_text_color(s_textlayer_minute, GColorWhite);
	} else{
		// set bg white and text black
		window_set_background_color(g_window, GColorWhite);
		text_layer_set_background_color(s_textlayer_hour, GColorWhite);	
		text_layer_set_background_color(s_textlayer_minute, GColorWhite);
		text_layer_set_text_color(s_textlayer_hour, GColorBlack);
		text_layer_set_text_color(s_textlayer_minute, GColorBlack);
	}
}
 
void init()
{
	//Create the app elements here!
	g_window = window_create();
	window_set_window_handlers(g_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)tick_handler);
	bluetooth_connection_service_subscribe((BluetoothConnectionHandler)bluetooth_handler);

	window_stack_push(g_window, true);
}
 
void deinit()
{
	//Destroy elements here to save memory!
	tick_timer_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	window_destroy(g_window);
}
 
int main(void)
{
	init();
	app_event_loop();
	deinit();
}