/*
	Pebble screen dimensions: 144 x 168 pixels
	https://github.com/Katharine/pebble-stopwatch/blob/laps/src/stopwatch.c
	
*/


#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PDUtils.h"
	
#include "resource_ids.auto.h"
	
#define UUID { 0x88, 0xa9, 0x7c, 0xfd, 0x23, 0x77, 0x46, 0x3a, 0x84, 0xbd, 0xb8, 0x5e, 0xb4, 0x96, 0x61, 0x61 }

PBL_APP_INFO(UUID, "Holiday Countdown", "g3rg", 0, 1, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;
TextLayer timeLayer;
TextLayer countdownLayer;
TextLayer eventLayer;

static char hourText[] = "00:00";

static int EVENT_YEAR = 2014;
static int EVENT_MONTH = 3;
static int EVENT_DAY = 2;
static int EVENT_HOUR = 0;
static int EVENT_MINUTE = 0;

static char EVENT_LABEL[255] = "Japan!";
static char COUNTDOWN_LABEL[255] = "0 days";

bool calculate_countdown() {
	PblTm now;
    PblTm event;
	time_t seconds_now;
	time_t seconds_event;
	int difference;
	char *time_format;
	static char countText[] = "";
	
	get_time(&now);
	
	seconds_now = pmktime(&now);
	
	event.tm_year = EVENT_YEAR - 1900;
	event.tm_mon = EVENT_MONTH - 1;
	event.tm_mday = EVENT_DAY;
	event.tm_hour = EVENT_HOUR;
	event.tm_min = EVENT_MINUTE;
	event.tm_sec = 0;
	
	seconds_event = pmktime(&event);
	
	difference = ((((seconds_event - seconds_now) / 60) / 60) / 24);
	
	if(difference < 0) {
		difference = 0;
	}
	
	snprintf(countText, 100, "%d days", difference);
	
	text_layer_set_text(&countdownLayer, countText);
	return true;
}


void setTime(PblTm *t) {
	 
	//If user selects '24hr' in Settings on the watch
	if(clock_is_24h_style())
		string_format_time(hourText, sizeof(hourText), "%H:%M", t);
	else
		string_format_time(hourText, sizeof(hourText), "%I:%M", t);
	 
	text_layer_set_text(&timeLayer, hourText);
	text_layer_set_text(&eventLayer, EVENT_LABEL);
	calculate_countdown();
}

void setImage(BmpContainer *container, const int resourceId, GRect bounds) {
	/*
	layer_remove_from_parent(&container->layer.layer);
	bmp_deinit_container(container);
	
	bmp_init_container(resourceId, container);
	
	layer_set_frame(&container->layer.layer, bounds);
	layer_add_child(window->layer, &container->layer.layer);
	*/
}

void handle_init(AppContextRef ctx) {
	(void)ctx;    //This is not needed. Convert to void (nothing)
    resource_init_current_app(&APP_RESOURCES);
	
	//Initialise window
	window_init(&window, "Window Name");
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);
	 
	//Initialise TextLayers
	text_layer_init(&timeLayer, GRect(30, 5, 114, 30));
	text_layer_set_background_color(&timeLayer, GColorClear);
	text_layer_set_text_color(&timeLayer, GColorWhite);
	text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(&timeLayer, GTextAlignmentLeft);
	 
	text_layer_init(&eventLayer, GRect(5, 30, 139, 60));
	text_layer_set_background_color(&eventLayer, GColorClear);
	text_layer_set_text_color(&eventLayer, GColorWhite);
	text_layer_set_font(&eventLayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(&eventLayer, GTextAlignmentCenter);
	
	text_layer_init(&countdownLayer, GRect(5, 130, 139, 38));
	text_layer_set_background_color(&countdownLayer, GColorClear);
	text_layer_set_text_color(&countdownLayer, GColorWhite);
	text_layer_set_font(&countdownLayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_alignment(&countdownLayer, GTextAlignmentCenter);
	
	//Add to window
	layer_add_child(&window.layer, &timeLayer.layer);
	layer_add_child(&window.layer, &eventLayer.layer);
	layer_add_child(&window.layer, &countdownLayer.layer);
	 
	//Set initial time so display isn't blank
	PblTm time;
	get_time(&time);
	setTime(&time);
	/*
	BmpContainer container;
	int width = 144;
	int height = 78;
	setImage(&container, RESOURCE_ID_SNOWBOARD_008, GRect(0, 30, width, height) );*/
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;
	 
	PblTm time;    //Structure to store time info
	get_time(&time);    //Fill the structure with current time
	setTime(t->tick_time);    //Change time on 'zero seconds' mark
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
	.init_handler = &handle_init,    //Register initialisation function
	 
	.tick_info = {
	.tick_handler = &handle_minute_tick,    //Register tick function
	.tick_units = MINUTE_UNIT    //Specify to call every minute
	}
	};
	app_event_loop(params, &handlers);    //Continue from there!
}


