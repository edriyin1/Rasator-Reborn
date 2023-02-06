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
#define display_MOSI 6
#define display_SCK 4
#define button 3

volatile bool ledState{ false };                // Backlight led state of LCD
volatile unsigned long lastInterruptTime{ 0 };  // Button debounce

Adafruit_BME280 bme280;
Adafruit_PCD8544 display{ display_SCK, display_MOSI, display_DC, display_CS, display_RST };

volatile unsigned long counter{ 0 };

void setup() {

  pinMode(3, INPUT);  // Button input

  //Timer Interrupt setup
  noInterrupts();
  TCCR1A = 0;           //Reset entire TCCR1A register
  TCCR1B = 0;           //Reset entire TCCR1B register
  TCCR1B |= B00000101;  //Set CS12 to 1 so we get Prescalar = 1024
  TIMSK1 |= B00000010;  //Set OCIE1A to 1 so we enable compare match A
  OCR1A = 65535;        //Finally we set compare register A to this value
  interrupts();

  attachInterrupt(digitalPinToInterrupt(button), button_press, CHANGE);  // Button interrupt for backlight of LCD

  bme280.begin();
  bme280.setSampling(Adafruit_BME280::MODE_FORCED,  // single-read
                     Adafruit_BME280::SAMPLING_X1,  // temperature
                     Adafruit_BME280::SAMPLING_X1,  // pressure
                     Adafruit_BME280::SAMPLING_X1,  // humidity
                     Adafruit_BME280::FILTER_OFF);
  display.begin();
}

void loop() {

  bme280.takeForcedMeasurement();  // has no effect in normal mode
  printValues();
  //delay(60000);
}

ISR(TIMER1_COMPA_vect) {

  TCNT1 = 0;  //First, set the timer back to 0 so it resets for next interrupt
  counter++;
}

void button_press() {

  const unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 200) {
    if (digitalRead(button) == HIGH) {
      ledState = !ledState;
    }
    if (ledState == true) {
      digitalWrite(display_LED, HIGH);
    } else {
      digitalWrite(display_LED, LOW);
    }
    lastInterruptTime = interruptTime;
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