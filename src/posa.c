#include "pebble.h"

#define ACCEL_STEP_MS 20000 // 20 sec
#define GRACE_PERIOD 1200000 // 20 minutes
#define FIRST_WARN 1
#define SECOND_WARN 3
#define THIRD_WARN 5
#define DEBUG false

static Window *s_main_window;
static TextLayer *s_text_layer;

uint ticks_on_back;
uint callback_time;

uint first_warns;
uint second_warns;
uint third_warns;

static bool supine_position(AccelData accel)
{
  return (accel.z < -600 && accel.y <= 700 && accel.y >= -700);
}

static void timer_callback(void *data)
{
  AccelData accel = (AccelData){.x = 0, .y = 0, .z = 0};
  accel_service_peek(&accel);

  if (supine_position(accel))
  {
    ticks_on_back += 1;
    switch (ticks_on_back)
    {
    case FIRST_WARN ... SECOND_WARN - 1:
      vibes_double_pulse();
      callback_time = ACCEL_STEP_MS - FIRST_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN));
      first_warns += 1;
      break;

    case SECOND_WARN ... THIRD_WARN - 1:
      vibes_short_pulse();
      callback_time = ACCEL_STEP_MS - SECOND_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN));
      second_warns += 1;
      break;

    case THIRD_WARN ... 1000:
      vibes_long_pulse();
      callback_time = ACCEL_STEP_MS - THIRD_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN));
      third_warns += 1;
      break;

    default:
      break;
    }
  }
  else
  {
    ticks_on_back = 0;
    callback_time = ACCEL_STEP_MS;
  };

  if (DEBUG)
  {
    static char s_buffer[64];
    snprintf(s_buffer, sizeof(s_buffer), "x:%d\ny:%d\nz:%d\nct:%d\nt:%d", accel.x, accel.y, accel.z, callback_time, ticks_on_back);
    text_layer_set_text(s_text_layer, s_buffer);
  }
  else
  {
    static char s_buffer[32];
    snprintf(s_buffer, sizeof(s_buffer), "\n1 - 2 - 3\n-------\n%d - %d - %d", first_warns, second_warns, third_warns);
    text_layer_set_text(s_text_layer, s_buffer);
  }

  app_timer_register(callback_time, timer_callback, NULL);
}

static void init()
{
  s_main_window = window_create();
  window_stack_push(s_main_window, true);

  ticks_on_back = 0;
  callback_time = ACCEL_STEP_MS;

  first_warns = 0;
  second_warns = 0;
  third_warns = 0;

  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(bounds);
  static char s_buffer[48];
  snprintf(s_buffer, sizeof(s_buffer), "Get ready to sleep!\n\nMonitoring in %d minutes.", GRACE_PERIOD / 1000 / 60);
  text_layer_set_text(s_text_layer, s_buffer);
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  // accel_data_service_subscribe(0, NULL);

  app_timer_register(GRACE_PERIOD, timer_callback, NULL);
}

static void deinit()
{
  text_layer_destroy(s_text_layer);
  window_destroy(s_main_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}