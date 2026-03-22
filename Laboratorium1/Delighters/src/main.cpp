#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const unsigned long READ_INTERVAL = 2000; 

const int ONE_WIRE_BUS = 14;
const String DEVICE_ID = "ESP_SENSOR_01";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long previousMillis = 0;
int healthCounter = 0;

void setup() {
  Serial.begin(9600);
  sensors.begin();

  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("System Start...");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= READ_INTERVAL) {
    previousMillis = currentMillis;

    sensors.requestTemperatures(); 
    float tempC = sensors.getTempCByIndex(0);

    String status;
    String value;

    if (tempC == DEVICE_DISCONNECTED_C) {
      healthCounter++;
      status = "ERR"; 
      value = "NaN";
    } else {
      healthCounter = 0;
      status = "OK";
      value = String(tempC, 1); 
    }
    Serial.print(currentMillis);
    Serial.print(", ");
    Serial.print(DEVICE_ID);
    Serial.print(", ");
    Serial.print(value);
    Serial.print(", ");
    Serial.print(status);
    if (healthCounter > 0) {
      Serial.print(", HC: ");
      Serial.print(healthCounter);
    }
    Serial.println();
    lcd.clear(); 
    
    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(value);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("STATUS: ");
    lcd.print(status);
  }
}
