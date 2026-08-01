#include <pebble_worker.h>
#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

static HealthValue heart_rate = 0;
static int update_rate = 15;




static void set_update_rate(){
     bool success = health_service_set_heart_rate_sample_period(update_rate);
}


static void worker_message_handler(uint16_t type, AppWorkerMessage *message)
{
    if (type == SOURCE_FOREGROUND)
    {
        update_rate = message->data0;
        set_update_rate();
    }
}

static void prv_on_health_data(HealthEventType type, void *context)
{

    // If the update was from the Heart Rate Monitor, query it
    if (type == HealthEventHeartRateUpdate)
    {
        heart_rate = health_service_peek_current_value(HealthMetricHeartRateBPM);
        AppWorkerMessage message = {
            .data0 = heart_rate};

        app_worker_send_message(SOURCE_BACKGROUND, &message);
    }
}


static void prv_init()
{
    
    set_update_rate();
    health_service_events_subscribe(prv_on_health_data, NULL);
    app_worker_message_subscribe(worker_message_handler);
}

static void prv_deinit()
{
    // Deinitialize the worker here
}

int main(void)
{
    prv_init();
    worker_event_loop();
    prv_deinit();
}