#include <Wire.h>

// RGB LED pins
#define LED_RED_PIN   9
#define LED_GREEN_PIN 10
#define LED_BLUE_PIN  11

// TCS34725 I2C address and registers
#define TCS_ADDR        0x29
#define TCS_CMD_BIT      0x80
#define TCS_CMD_AUTOINC  0x20

#define TCS_REG_ENABLE  0x00
#define TCS_REG_ATIME   0x01
#define TCS_REG_CONTROL 0x0F
#define TCS_REG_ID      0x12
#define TCS_REG_CDATAL  0x14  // Clear, Red, Green, Blue data start (8 bytes total)

#define TCS_ENABLE_PON  0x01
#define TCS_ENABLE_AEN  0x02

void tcsWrite8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(TCS_ADDR);
  Wire.write(TCS_CMD_BIT | reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t tcsRead8(uint8_t reg) {
  Wire.beginTransmission(TCS_ADDR);
  Wire.write(TCS_CMD_BIT | reg);
  Wire.endTransmission();
  Wire.requestFrom(TCS_ADDR, 1);
  return Wire.read();
}

void tcsReadAllChannels(uint16_t &c, uint16_t &r, uint16_t &g, uint16_t &b) {
  Wire.beginTransmission(TCS_ADDR);
  Wire.write(TCS_CMD_BIT | TCS_CMD_AUTOINC | TCS_REG_CDATAL);
  Wire.endTransmission();
  Wire.requestFrom(TCS_ADDR, 8);

  c = Wire.read() | (Wire.read() << 8);
  r = Wire.read() | (Wire.read() << 8);
  g = Wire.read() | (Wire.read() << 8);
  b = Wire.read() | (Wire.read() << 8);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  // Verify sensor is present (ID register should read 0x44 or 0x4D)
  uint8_t id = tcsRead8(TCS_REG_ID);
  if (id == 0x44 || id == 0x4D) {
    Serial.println("TCS34725 color sensor found.");
  } else {
    Serial.println("No TCS34725 found ... check your wiring!");
    while (1); // halt
  }

  tcsWrite8(TCS_REG_ATIME, 0xEB);     // Integration time ~50ms
  tcsWrite8(TCS_REG_CONTROL, 0x01);   // Gain 4x
  tcsWrite8(TCS_REG_ENABLE, TCS_ENABLE_PON);
  delay(3);
  tcsWrite8(TCS_REG_ENABLE, TCS_ENABLE_PON | TCS_ENABLE_AEN);
  delay(60); // wait for first integration cycle
}

void loop() {
  uint16_t c, r, g, b;
  tcsReadAllChannels(c, r, g, b);

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
