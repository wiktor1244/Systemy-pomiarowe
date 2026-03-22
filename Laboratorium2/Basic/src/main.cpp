#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_BMP085.h>

ESP8266WebServer server(80);
Adafruit_BMP085 bmp;

void handleSensor() {
  float temp = bmp.readTemperature(); 

  String json = "{";
  json += "\"temp\": ";
  json += String(temp, 2); 
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

  Serial.print("Adres IP serwera (AP): ");
  Serial.println(WiFi.softAPIP());

  server.on("/sensor", handleSensor); 
  server.begin(); 
}

void loop() {
  server.handleClient(); 

  float temp = bmp.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" *C");

  delay(2000); 
}
