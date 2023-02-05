#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define display_DC 9
#define display_CS 10
#define display_RST 8
#define display_LED 5

#define button 3

volatile bool isPressed{ false };
volatile unsigned long last_interrupt_time{ 0 };
Adafruit_BME280 bme280;
Adafruit_PCD8544 display{ display_DC, display_CS, display_RST };

void setup() {

  pinMode(3, INPUT);
  bme280.begin();
  bme280.setSampling(Adafruit_BME280::MODE_FORCED,  // single-read
                     Adafruit_BME280::SAMPLING_X1,  // temperature
                     Adafruit_BME280::SAMPLING_X1,  // pressure
                     Adafruit_BME280::SAMPLING_X1,  // humidity
                     Adafruit_BME280::FILTER_OFF);
  display.begin();
  attachInterrupt(digitalPinToInterrupt(button), button_press, CHANGE);
}

void loop() {

  bme280.takeForcedMeasurement();  // has no effect in normal mode
  printValues();
  delay(60000);
}

void button_press() {

  const unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200) {
    if (digitalRead(button) == HIGH) {
      isPressed = !isPressed;
    }
    if (isPressed == true) {
      digitalWrite(display_LED, HIGH);
    } else {
      digitalWrite(display_LED, LOW);
    }
    last_interrupt_time = interrupt_time;
  }
}

void printValues() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Temperature:");
  display.print(bme280.readTemperature());
  display.println(" *C");
  display.println("Humidity:");
  display.print(bme280.readHumidity());
  display.println(" %");
  display.display();
}