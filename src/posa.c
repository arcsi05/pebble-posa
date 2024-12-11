#include "pebble.h"

// Time interval between each accelerometer measurement (20 seconds)
#define ACCEL_STEP_MS 20000
// Time period for which no haptic feedback is given before starting to monitor (20 minutes)
#define GRACE_PERIOD 1200000
// First warning tick value
#define FIRST_WARN 1
// Second warning tick value
#define SECOND_WARN 3
// Third warning tick value
#define THIRD_WARN 5

// Flag to enable or disable debugging messages
#define DEBUG false

static Window *s_main_window;   // Main window for the application
static TextLayer *s_text_layer; // Text layer to display warnings and status

uint ticks_on_back; // Counter to track time spent in supine position
uint callback_time; // Time until the next timer callback

// Variables to keep track of warning counts
uint first_warns;
uint second_warns;
uint third_warns;

/**
 * Checks if the user is in a supine (back) position based on accelerometer data.
 *
 * @param accel Accelerometer data
 * @return true if the user is in a supine position, false otherwise
 */
static bool supine_position(AccelData accel)
{
  return (accel.z < -600 && accel.y <= 700 && accel.y >= -700);
}

/**
 * Timer callback function to handle timing and checking for supine position.
 */
static void timer_callback()
{
  // Read the latest accelerometer data
  AccelData accel = (AccelData){.x = 0, .y = 0, .z = 0};
  accel_service_peek(&accel);

  if (supine_position(accel))
  {
    ticks_on_back += 1; // Increment counter for time spent in supine position

    // Determine warning level based on number of ticks
    switch (ticks_on_back)
    {
    case FIRST_WARN ... SECOND_WARN - 1:
      vibes_double_pulse();
      callback_time = ACCEL_STEP_MS - FIRST_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN)); // Reduce time between checks
      first_warns += 1; // Increment first warning counter
      break;

    case SECOND_WARN ... THIRD_WARN - 1:
      vibes_short_pulse();
      callback_time = ACCEL_STEP_MS - SECOND_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN)); // Reduce time between checks
      second_warns += 1; // Increment second warning counter
      break;

    case THIRD_WARN ... 1000:
      vibes_long_pulse();
      callback_time = ACCEL_STEP_MS - THIRD_WARN * (ACCEL_STEP_MS / (FIRST_WARN + SECOND_WARN + THIRD_WARN)); // Reduce time between checks
      third_warns += 1; // Increment third warning counter
      break;

    default:
      break;
    }
  }
  else
  {
    ticks_on_back = 0;             // Reset counter for new measurement
    callback_time = ACCEL_STEP_MS; // Reset time between checks
  };

  if (DEBUG)
  {
    static char s_buffer[64]; // Buffer to store debugging messages
    snprintf(s_buffer, sizeof(s_buffer), "x:%d\ny:%d\nz:%d\nct:%d\nt:%d", accel.x, accel.y, accel.z, callback_time, ticks_on_back);
    text_layer_set_text(s_text_layer, s_buffer); // Display accelerometer data and time
  }
  else
  {
    static char s_buffer[32]; // Buffer to store warnings and status
    snprintf(s_buffer, sizeof(s_buffer), "\n1 - 2 - 3\n-------\n%d - %d - %d", first_warns, second_warns, third_warns);
    text_layer_set_text(s_text_layer, s_buffer); // Display warning counts
  }

  app_timer_register(callback_time, timer_callback, NULL); // Schedule next timer callback
}

/**
 * Initializes the application.
 */
static void init()
{
  // Create and push the main window onto the stack
  s_main_window = window_create();
  window_stack_push(s_main_window, true);

  // Initialize variables for tracking time spent in supine position and warning counters
  ticks_on_back = 0;
  callback_time = ACCEL_STEP_MS;

  first_warns = 0;
  second_warns = 0;
  third_warns = 0;

  // Get the bounds of the main window layer
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create and add a text layer to display warnings and status
  s_text_layer = text_layer_create(bounds);
  static char s_buffer[48]; // Buffer for introduction message
  snprintf(s_buffer, sizeof(s_buffer), "Get ready to sleep!\n\nMonitoring in %d minutes.", GRACE_PERIOD / 1000 / 60);
  text_layer_set_text(s_text_layer, s_buffer); // Display introduction message
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  // Register the timer callback function with a duration of GRACE_PERIOD
  app_timer_register(GRACE_PERIOD, timer_callback, NULL);
}

/**
 * Cleans up resources and destroys the main window.
 */
static void deinit()
{
  // Destroy the text layer
  text_layer_destroy(s_text_layer);
  // Destroy the main window
  window_destroy(s_main_window);
}

int main(void)
{
  init();           // Initialize the application
  app_event_loop(); // Start the event loop
  deinit();         // Clean up resources and destroy the main window on exit
}