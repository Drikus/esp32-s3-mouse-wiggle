#include <Arduino.h>
#include <FastLED.h>
#include <USB.h>
#include <USBHIDMouse.h>
#include <math.h>

#define LED_PIN     48
#define NUM_LEDS    1

USBHIDMouse Mouse;

CRGB leds[NUM_LEDS];

// Brightness (leave at 255 by default)
static const uint8_t DEFAULT_BRIGHTNESS = 255;

// Helper: set color and apply brightness
void setLedColor(const CRGB &color, uint8_t brightness = DEFAULT_BRIGHTNESS) {
  FastLED.setBrightness(brightness);
  leds[0] = color;
  FastLED.show();
}

// Blink LED red 6 times over ~2 seconds
void blinkRed6() {
  const uint16_t blinkTotalMs = 2000;
  const uint8_t blinks = 6;
  const uint16_t halfPeriod = blinkTotalMs / blinks / 2; // on/off half periods
  for (uint8_t i = 0; i < blinks; ++i) {
    setLedColor(CRGB::Red);
    delay(halfPeriod);
    setLedColor(CRGB::Black);
    delay(halfPeriod);
  }
}

// Wiggle mouse for a short randomized duration while LED is blue
void wiggleMouse() {
  // wiggle duration between 800ms and 2000ms
  uint16_t wiggleMs = 800 + (random(0, 1201));
  unsigned long start = millis();
  setLedColor(CRGB::Blue);
  while (millis() - start < wiggleMs) {
    int8_t dx = (int8_t)random(-3, 4); // -3..3
    int8_t dy = (int8_t)random(-3, 4);
    Mouse.move(dx, dy);
    delay(60);
  }
  setLedColor(CRGB::Black);
}

void setup() {
 FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
 // Initialize USB HID Mouse
 USB.begin();
 Mouse.begin();
}

void loop() {
  // Wait roughly one minute +/- jitter before the next wiggle
  const uint32_t baseMs = 60000UL; // 60s
  const uint32_t jitterMs = 10000UL; // +/-10s
  int32_t r = (int32_t)random(0, (int32_t)jitterMs * 2 + 1) - (int32_t)jitterMs; // -jitter..+jitter
  uint32_t waitMs = baseMs + (int32_t)r;
  // avoid extremely small waits
  if (waitMs < 1000) waitMs = baseMs;
  delay(waitMs);

  // Blink red 6 times over ~2s to warn
  blinkRed6();

  // Perform the mouse wiggle while LED is blue
  wiggleMouse();
}
