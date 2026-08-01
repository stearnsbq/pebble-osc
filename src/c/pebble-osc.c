#include <pebble.h>

#define SETTINGS_KEY 1
#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

#define EVENT_CONNECTED 0
#define EVENT_SOCKET_CLOSED 1
#define EVENT_CONNECT 2
#define EVENT_HEART_RATE 3
#define EVENT_DISCONNECT 4

static Window *s_window;
static TextLayer *s_text_layer;
static TextLayer *sub_text_layer;

typedef struct PulsoidSettings
{
  char *APIToken;
  int UpdateRate;
} PulsoidSettings;

static PulsoidSettings settings;

static void worker_message_handler(uint16_t type,
                                   AppWorkerMessage *message)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Recieved worker message");
  if (type == SOURCE_BACKGROUND)
  {
    int heart_rate = message->data0;

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    dict_write_int32(iter, MESSAGE_KEY_HeartRate, heart_rate);

    app_message_outbox_send();
  }
}

static void prv_load_settings()
{
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));

  AppWorkerMessage message = {
      .data0 = settings.UpdateRate};

  app_worker_send_message(SOURCE_FOREGROUND, &message);
}

// Save the settings to persistent storage
static void prv_save_settings()
{
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));

  AppWorkerMessage message = {
      .data0 = settings.UpdateRate};

  app_worker_send_message(SOURCE_FOREGROUND, &message);
}

// AppMessage receive handler
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context)
{

  Tuple *update_rate_t = dict_find(iter, MESSAGE_KEY_UpdateRate);
  if (update_rate_t)
  {
    settings.UpdateRate = update_rate_t->value->int32;
  }

  Tuple *event_t = dict_find(iter, MESSAGE_KEY_Event);
  if (event_t)
  {
    int event = event_t->value->int32;

    switch (event)
    {
    case EVENT_SOCKET_CLOSED:
    {
      text_layer_set_text(s_text_layer, "Relay Disconnected");
      text_layer_set_text(sub_text_layer, "Hit Select to reconnect");
      break;
    }
    case EVENT_CONNECTED:
    {
      text_layer_set_text(s_text_layer, "Connected");
      text_layer_set_text(sub_text_layer, "");
      break;
    }
    }
  }

  prv_save_settings();
}

static void send_connect_message()
{
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if (result == APP_MSG_OK)
  {
    int evt = EVENT_CONNECT;
    // Construct the message
    dict_write_int32(out_iter, MESSAGE_KEY_Event, EVENT_CONNECT);
    result = app_message_outbox_send();
    if (result != APP_MSG_OK)
    {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  }
  else
  {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void send_disconnect_message()
{
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if (result == APP_MSG_OK)
  {
    int evt = EVENT_CONNECT;
    // Construct the message
    dict_write_int32(out_iter, MESSAGE_KEY_Event, EVENT_CONNECT);
    result = app_message_outbox_send();
    if (result != APP_MSG_OK)
    {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  }
  else
  {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // text_layer_set_text(s_text_layer, "Select");
  text_layer_set_text(s_text_layer, "Connecting to Relay");
  text_layer_set_text(sub_text_layer, "");
  send_connect_message();
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context)
{
  // text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  sub_text_layer = text_layer_create(GRect(0, 92, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "Connecting to Relay");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(sub_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(sub_text_layer));
  AppWorkerResult result = app_worker_launch();
   app_worker_message_subscribe(worker_message_handler);
}

static void prv_window_unload(Window *window)
{
  text_layer_destroy(s_text_layer);
}

static void prv_init(void)
{
  prv_load_settings();
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });
  const bool animated = true;
  window_stack_push(s_window, animated);

  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
}

static void prv_deinit(void)
{
  window_destroy(s_window);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
