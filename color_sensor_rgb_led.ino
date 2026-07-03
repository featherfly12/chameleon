/*
  Color Sensor (TCS34725) + RGB LED
  --------------------------------
  Wiring:
    Color sensor:
      SCL -> Arduino A5 (SCL)
      SDA -> Arduino A4 (SDA)
      PWR -> 5V (or 3.3V, check your board's spec)
      GND -> GND
    RGB LED (common cathode):
      Red   -> pin 9  (through a ~220ohm resistor)
      Green -> pin 10 (through a ~220ohm resistor)
      Blue  -> pin 11 (through a ~220ohm resistor)
      Common cathode -> GND

  Library required:
    Adafruit TCS34725
    Install via: Arduino IDE -> Sketch -> Include Library -> Manage Libraries
    -> search "Adafruit TCS34725" -> Install
*/

#include <Wire.h>
#include "Adafruit_TCS34725.h"

// RGB LED pins
#define LED_RED_PIN   9
#define LED_GREEN_PIN 10
#define LED_BLUE_PIN  11

// Create sensor object (adjust integration time / gain if readings are too dim/bright)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  if (tcs.begin()) {
    Serial.println("TCS34725 color sensor found.");
  } else {
    Serial.println("No TCS34725 found ... check your wiring!");
    while (1); // halt
  }
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  int redVal = 0, greenVal = 0, blueVal = 0;

  // Normalize raw channel values to 0-255 using the clear (c) channel
  if (c > 0) {
    redVal   = constrain((int)((float)r / c * 255.0), 0, 255);
    greenVal = constrain((int)((float)g / c * 255.0), 0, 255);
    blueVal  = constrain((int)((float)b / c * 255.0), 0, 255);
  }

  // Print raw and normalized values to Serial Monitor
  Serial.print("Raw  -> R: "); Serial.print(r);
  Serial.print("  G: "); Serial.print(g);
  Serial.print("  B: "); Serial.print(b);
  Serial.print("  C: "); Serial.println(c);

  Serial.print("RGB  -> R: "); Serial.print(redVal);
  Serial.print("  G: "); Serial.print(greenVal);
  Serial.print("  B: "); Serial.println(blueVal);
  Serial.println("--------------------------");

  // Drive the RGB LED to visually match the detected color
  analogWrite(LED_RED_PIN, redVal);
  analogWrite(LED_GREEN_PIN, greenVal);
  analogWrite(LED_BLUE_PIN, blueVal);

  delay(500);
}
