#include <HCSR04.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define IRQ_PIN 2
#define XSHUT_PIN 3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

UltraSonicDistanceSensor distanceSensor(12, 6);  // Initialize sensor that uses digital (trig, echo)

int lightSensorPin = 10;
int sensorValue;
int sensorMin = INT_MAX;
int sensorMax = INT_MIN;
float distance = 0;
char* idleText = "No reading available\n";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Calibrate the light sensor
  while (millis() < 5000) {
    sensorValue = analogRead(lightSensorPin);
    // record the minimum and maximum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }
  }

  Wire.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
}

void loop() {
  // Check to see if fist is closed
  while (analogRead(lightSensorPin) < ((sensorMax - sensorMin)/2)){
    drawDistance();
  }
  drawIdle();
}

// Display distance
void drawDistance() {
  display.clearDisplay();
  display.setTextSize(2);      // 3:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Actually display the distance
  distance = distanceSensor.measureDistanceCm();
  char str[30];
  dtostrf(distanceSensor.measureDistanceCm(), 3, 2, str);
  Serial.println(distance);
  Serial.println(str);
  int i = 0;
  while (str[i] != '\0') {
    display.write(str[i]);
    i++;
  }
  display.write(' ');
  display.write(99);
  display.write(109);

  display.display();
  delay(500);
}

// Display Idle
void drawIdle() {
  display.clearDisplay();
  display.setTextSize(1);      // 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Display idle text
  int i = 0;
  while (idleText[i] != '\n') {
    display.write(idleText[i]);
    i++;
  }
  display.write(' ');

  display.display();
  delay(500);
}
