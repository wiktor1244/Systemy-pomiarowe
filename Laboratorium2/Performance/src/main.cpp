#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BMP085.h>

ESP8266WebServer server(80);
Adafruit_BMP085 bmp;

unsigned long previousMillis = 0;
const long interval = 2000; 

unsigned long lastRequestTime = 0;

void handleSensor() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastRequestTime < 2000) {
    server.send(429, "text/plain", "429 Too Many Requests");
    return; 
  }
  lastRequestTime = currentMillis;

  Wire.beginTransmission(0x77);
  byte error = Wire.endTransmission();
  
  if (error != 0) {
    server.send(503, "text/plain", "503 Service Unavailable");
    return; 
  }

  float temp = bmp.readTemperature(); 
  int32_t press = bmp.readPressure(); 

  String json = "{";
  json += "\"temp\": " + String(temp, 2) + ", ";
  json += "\"pressurePa\": " + String(press);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(2000); 

  Wire.begin(4, 5); 
  bmp.begin();

  WiFi.mode(WIFI_AP); 
  WiFi.softAP("Stacja-LAB-NR_INDEXU", "inzynier2026"); 

  Serial.println("\n--- BRAMA POWIETRZNA (PERFORMANCE) ---");
  Serial.print("Adres IP bramy (AP): ");
  Serial.println(WiFi.softAPIP());

  server.on("/sensor", handleSensor);
  server.begin();
}

void loop() {
  server.handleClient(); 

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Wire.beginTransmission(0x77);
    if (Wire.endTransmission() == 0) {
      float temp = bmp.readTemperature();
      int32_t press = bmp.readPressure();
      
      Serial.print("[LOG] Temp: ");
      Serial.print(temp);
      Serial.print(" *C | Cisnienie: ");
      Serial.print(press);
      Serial.println(" Pa");
    } else {
      Serial.println("[LOG] KRYTYCZNE: Błąd I2C - Brak komunikacji z BMP180!");
    }
  }
}
