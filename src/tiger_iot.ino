/**
   PostHTTPClient.ino

    Created on: 21.11.2016

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h> 
#include <ArduinoJson.h>

#include "thermister.h"

#define USE_SERIAL Serial

#define SERVER_IP "192.168.1.69:6969"
#define END_POINT "/home_api/room_temp"
#define DEVICE_NAME "bedroom"
#define DELAY_PER_DATAPOINT 600000


void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.println();

  WiFiManager wifiManager;
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect("TigerIoTAutoConectAP", "password");
}

void loop() {
  // Read sensor values
  float temp = readThermister();
  USE_SERIAL.print("Temperature (C): ");
  USE_SERIAL.println(temp);

  // Construct JSON payload
  DynamicJsonDocument doc(200);
  doc["name"] = DEVICE_NAME;
  doc["value"] = temp;

  // Convert to String
  String post_payload;
  serializeJson(doc, post_payload);

  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    USE_SERIAL.print("[HTTP] Beginning connection to ");
    USE_SERIAL.println(SERVER_IP END_POINT);
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP END_POINT); //HTTP
    http.addHeader("Content-Type", "application/json");

    // start connection and send HTTP header and body
    int httpCode = http.POST(post_payload);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        const String& received_payload = http.getString();
        USE_SERIAL.println("received payload:\n<<");
        USE_SERIAL.println(received_payload);
        USE_SERIAL.println(">>");
      }
    } else {
      USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(DELAY_PER_DATAPOINT);
}
