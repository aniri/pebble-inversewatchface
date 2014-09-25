#include <pebble.h>
 
Window* g_window;
TextLayer *g_text_layer;

static GFont time_font;
static GFont date_font;
static TextLayer *s_textlayer_time;
static TextLayer *s_textlayer_weekday;
static TextLayer *s_textlayer_date;
 
void invertColors(bool connected)
{
	// invert colors
	if (connected){
		// set bg black and text white
		window_set_background_color(g_window, GColorBlack);
		text_layer_set_background_color(s_textlayer_time, GColorBlack);	
		text_layer_set_background_color(s_textlayer_weekday, GColorBlack);
		text_layer_set_background_color(s_textlayer_date, GColorBlack);
		text_layer_set_text_color(s_textlayer_time, GColorWhite);
		text_layer_set_text_color(s_textlayer_weekday, GColorWhite);
		text_layer_set_text_color(s_textlayer_date, GColorWhite);
	} else{
		// set bg white and text black
		window_set_background_color(g_window, GColorWhite);
		text_layer_set_background_color(s_textlayer_time, GColorWhite);	
		text_layer_set_background_color(s_textlayer_weekday, GColorWhite);
		text_layer_set_background_color(s_textlayer_date, GColorWhite);
		text_layer_set_text_color(s_textlayer_time, GColorBlack);
		text_layer_set_text_color(s_textlayer_weekday, GColorBlack);
		text_layer_set_text_color(s_textlayer_date, GColorBlack);
	}
}

void window_load(Window *window)
{
	time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
	date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	
	// s_textlayer_time
	s_textlayer_time = text_layer_create(GRect(10, 30, 125, 50));
	text_layer_set_text(s_textlayer_time, "00:00");
	text_layer_set_text_alignment(s_textlayer_time, GTextAlignmentCenter);
	text_layer_set_font(s_textlayer_time, time_font);
	layer_add_child(window_get_root_layer(window), (Layer *)s_textlayer_time);

	// s_textlayer_weekday
	s_textlayer_weekday = text_layer_create(GRect(10, 100, 125, 30));
	text_layer_set_text(s_textlayer_weekday, "      ");
	text_layer_set_text_alignment(s_textlayer_weekday, GTextAlignmentLeft);
	text_layer_set_font(s_textlayer_weekday, date_font);
	layer_add_child(window_get_root_layer(window), (Layer *)s_textlayer_weekday);

	// s_textlayer_date
	s_textlayer_date = text_layer_create(GRect(10, 130, 125, 30));
	text_layer_set_text(s_textlayer_date, "           ");
	text_layer_set_text_alignment(s_textlayer_date, GTextAlignmentRight);
	text_layer_set_font(s_textlayer_date, date_font);
	layer_add_child(window_get_root_layer(window), (Layer *)s_textlayer_date);
	
	invertColors(bluetooth_connection_service_peek());
}
 
void window_unload(Window *window)
{
	//safely destroy the Window's elements
	text_layer_destroy(s_textlayer_time);
    text_layer_destroy(s_textlayer_weekday);
    text_layer_destroy(s_textlayer_date);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	// allocate long-lived storage (required by TextLayer)
	static char buffer_time[] = "00:00";
	static char buffer_weekday[] = "           ";
	static char buffer_date[] = "              ";

	// write the time, weekday and date to the buffer in a safe manner
	strftime(buffer_time, sizeof("00:00"), "%H:%M", tick_time); 
	strftime(buffer_weekday, sizeof("           "), "%A", tick_time);
	strftime(buffer_date, sizeof("              "), "%d %B", tick_time);

	// set the TextLayers to display the buffer
	text_layer_set_text(s_textlayer_time, buffer_time);
	text_layer_set_text(s_textlayer_weekday, buffer_weekday);
	text_layer_set_text(s_textlayer_date, buffer_date);
}

void bluetooth_handler(bool connected)
{
	invertColors(connected);
}
 
void init()
{
	//Create the app elements here!
	g_window = window_create();
	window_set_window_handlers(g_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	// subscribe to tick timer
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)tick_handler);
	// subscribe t obluetooth service for checking connection
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