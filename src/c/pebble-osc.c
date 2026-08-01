#include <pebble.h>

#define SETTINGS_KEY 1
#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

static Window *s_window;
static TextLayer *s_text_layer;


typedef struct PulsoidSettings {
  char* APIToken;
  int UpdateRate;
} PulsoidSettings;


static PulsoidSettings settings;

static void prv_load_settings() {
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));


    AppWorkerMessage message = {
    .data0 = settings.UpdateRate
  };

  app_worker_send_message(SOURCE_FOREGROUND, &message);

}


// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));


  AppWorkerMessage message = {
    .data0 = settings.UpdateRate
  };

  app_worker_send_message(SOURCE_FOREGROUND, &message);
} 


// AppMessage receive handler
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
 
  // Assign the values to our struct
  Tuple *api_token_t = dict_find(iter, MESSAGE_KEY_APIToken);
  if (api_token_t) {
    settings.APIToken = (char*)api_token_t->value;
  }

  Tuple *update_rate_t = dict_find(iter, MESSAGE_KEY_UpdateRate);
  if (api_token_t) {
    settings.UpdateRate = update_rate_t->value->int32;
  }

  prv_save_settings();
}




static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "Press a button");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
    AppWorkerResult result = app_worker_launch();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  prv_load_settings();
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);

    app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
