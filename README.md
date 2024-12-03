# Positional Sleep Apnea Monitor for Pebble Watch

This application is designed for the Pebble smartwatch to assist individuals with **Positional Sleep Apnea** in improving their sleep quality. The app uses the watch's accelerometer to detect when the user is sleeping on their back (supine position) and delivers haptic feedback to encourage them to change their sleeping position.

---

## Features

- **Supine Detection**: Monitors the user's sleeping position using the Pebble's built-in accelerometer.
- **Haptic Feedback**: Provides escalating haptic warnings when the user remains on their back:
  - Double pulse: Initial gentle warning.
  - Short pulse: Secondary stronger warning.
  - Long pulse: Persistent final warning.
- **Adjustable Grace Period**: Allows for a 20-minute grace period before monitoring begins to ensure the user has time to settle into sleep.
- **Real-Time Metrics**: Displays warning counts for tracking intervention events.

---

## How It Works

1. **Setup and Monitoring**:
   - Once the app is launched, it starts with a grace period of 20 minutes.
   - After the grace period, the accelerometer samples data every 20 seconds to detect the user's orientation.

2. **Supine Detection**:
   - The app identifies a supine position based on accelerometer data (`z-axis` value below -600 and `y-axis` values within ±700).

3. **Haptic Feedback**:
   - The longer the user stays on their back, the more intense the warnings become, encouraging a position change.

4. **Feedback Display**:
   - The Pebble's screen shows the count of each type of warning delivered (1st, 2nd, and 3rd warnings).

---

## Build and Install

> There are plans to submit the application to the [Rebble Store for Pebble](http://store-beta.rebble.io/apps), but this will only happen when additional features are added and stable.

1. Clone the repository:
   ```bash
   git clone https://github.com/arcsi05/pebble-posa.git
   cd pebble-posa
   ```
2. Run the setup script
   ```bash
   ./dev_env_setup.sh
   ```

3. Build the app using the Pebble SDK:
   ```bash
   pebble build
   ```

4. Install on your physical Pebble watch using:
   ```bash
   pebble install --phone <PHONE IP ADDRESS>
   ```

---

## Code Overview

The core logic is implemented in C, utilizing Pebble's SDK. Key components include:

- **Timer-based Monitoring**: The `timer_callback` function periodically checks the user's orientation and triggers haptic feedback.
- **Acceleration Analysis**: The `supine_position` function evaluates accelerometer data to detect the supine position.
- **Warning System**: Escalating warnings managed via `ticks_on_back` counter and conditional statements.

---

## Usage Tips

- Wear the Pebble on your body (attach with tape or something) in a way that the screen faces upwards when you're on your back.
- Ensure the watch has sufficient battery to last through the night.
- Monitor feedback counts in the morning to assess positional interventions.

---

## Contributing

Contributions are welcome! If you have suggestions for improvement or new features, let me know!

