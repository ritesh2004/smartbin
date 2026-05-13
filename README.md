# Smartbin

## Overview

`smartbin` is an ESP32-based smart trash bin prototype that uses an AI-Thinker camera module for motion detection and an attached servo to automatically open and close the lid.

The system captures grayscale camera frames, compares each frame against the previous one, and triggers the lid servo when motion exceeds a threshold.

## Hardware

- ESP32-compatible board with camera support
- AI-Thinker camera module (`CAMERA_MODEL_AI_THINKER`)
- Servo motor connected to GPIO `12`
- LED indicator connected to GPIO `4`

## Software Behavior

- Initializes the ESP32 camera in grayscale mode at `FRAMESIZE_QQVGA`
- Captures an initial reference frame on startup
- Continuously captures frames in `loop()`
- Detects motion by comparing pixel differences between the current and previous frames
- If the number of changed pixels exceeds `MOTION_THRESHOLD`, the lid opens for 3 seconds then closes

## Configuration

Defined values in `Main.ino`:

- `PIXEL_THRESHOLD` = `25`
- `MOTION_THRESHOLD` = `200`
- `LED_PIN` = `4`
- `SERVO_PIN` = `12`

## Important Notes

- The camera is configured for grayscale operation to simplify motion detection.
- The servo is moved to `90` degrees to open and back to `0` degrees to close.
- The current implementation uses a simple frame differencing algorithm and may need tuning for ambient light or camera noise.

## Build / Flash

Use the Arduino IDE or PlatformIO with ESP32 support to build and upload `Main.ino` to the device.

1. Open the project in the Arduino IDE.
2. Select the correct ESP32 board.
3. Set the correct camera model if needed.
4. Upload the sketch.
