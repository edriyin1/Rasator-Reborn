#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define I2C_SDA 0
#define I2C_SCL 4

//Adafruit_PCD8544 display = Adafruit_PCD8544(18, 23, 14, 15, 2);
Adafruit_PCD8544 display{ 18, 23, 14, 15, 2 };

//TwoWire I2CBME = TwoWire(0);
TwoWire I2CBME{ 0 };

Adafruit_BME280 bme;  // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

void setup() {
  Serial.begin(115200);
  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.println(F("BME280 Sensor event test"));

  if (!bme.begin(0x77, &I2CBME)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1) delay(10);
  }

  display.begin();
  display.setContrast(10);

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
}

void loop() {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println(temp_event.temperature);
  display.println(humidity_event.relative_humidity);
  display.println(pressure_event.pressure);
  display.display();

  Serial.println(temp_event.temperature);
  Serial.println(humidity_event.relative_humidity);
  Serial.println(pressure_event.pressure);

  Serial.println();
  delay(1000);
}