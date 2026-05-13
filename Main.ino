#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "esp32-hal-ledc.h"
#include "sdkconfig.h"
#include "Arduino.h"
#include "esp_http_server.h"

#include <ESP32Servo.h>

// Camera model (uncomment your camera model)
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
#define CAMERA_MODEL_AI_THINKER  // Most common model

// Global variable to hold the HTTP server instance
httpd_handle_t camera_httpd = NULL;

// =====================
// MOTION SETTINGS
// =====================

#define PIXEL_THRESHOLD 25
#define MOTION_THRESHOLD 200

#define LED_PIN 4

#define SERVO_PIN 12

Servo lidServo;

bool motionDetected = false;

uint8_t *previousFrame = NULL;

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(LED_PIN, OUTPUT);

  lidServo.attach(SERVO_PIN);

  // Configure the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;

  // Small resolution for speed
  config.frame_size = FRAMESIZE_QQVGA;

  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  digitalWrite(LED_PIN, HIGH);

   // Capture first frame
  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  previousFrame = (uint8_t*)malloc(fb->len);

  memcpy(previousFrame, fb->buf, fb->len);

  esp_camera_fb_return(fb);
}

void loop() {
  // The loop function is intentionally left empty
  // The camera server is handling everything in the background
  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Capture failed");
    return;
  }

  motionDetected = detectMotion(fb);

  esp_camera_fb_return(fb);

  if (motionDetected) {

    // Serial.println("LID Open");
    openLid();

    // Cooldown
    delay(3000);
  }

  delay(100);
}

void openLid() {

  Serial.println("Opening Lid");

  lidServo.write(90);

  delay(3000);

  lidServo.write(0);

  Serial.println("Closing Lid");
}

bool detectMotion(camera_fb_t *fb) {

  int changedPixels = 0;

  // Skip pixels for speed
  for (int i = 0; i < fb->len; i += 4) {

    uint8_t currentPixel = fb->buf[i];
    uint8_t previousPixel = previousFrame[i];

    int diff = abs(currentPixel - previousPixel);

    if (diff > PIXEL_THRESHOLD) {
      changedPixels++;
    }
  }

  Serial.print("Changed Pixels: ");
  Serial.println(changedPixels);

  // Copy current frame into previous frame
  memcpy(previousFrame, fb->buf, fb->len);

  if (changedPixels > MOTION_THRESHOLD) {
    return true;
  }

  return false;
}
