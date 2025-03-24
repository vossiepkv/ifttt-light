#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 GY30; // instantiate a sensor event object

#include "Secret.h" 
// Please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // Your network SSID (name)
char pass[] = SECRET_PASS;    // Your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // The WiFi radio's status
WiFiClient  client;

// Defines IFTTT event webhook listeners
char   HOST_NAME[] = "maker.ifttt.com";
char   HOST_NAME1[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/sun-on/with/key/iAGT0WbFP-AP71q4ClRiDVukOYDMnBgMoOzvgGsaBN-";
String PATH_NAME1  = "/trigger/sun-off/with/key/iAGT0WbFP-AP71q4ClRiDVukOYDMnBgMoOzvgGsaBN-";


void setup() {
  Serial.begin(9600);

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000); // Wait 10 seconds for connection
  }
  Serial.println("Connected to WiFi");

  Wire.begin(); // Initialize the I2C bus  
  GY30.begin(); // Initialize the sensor object

  if (client.connect(HOST_NAME, 80)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Connection failed");
  }
}

bool aboveThresholdNotified = false; // Flag to track if notification for lux > 300 has been sent
bool belowThresholdNotified = false; // Flag to track if notification for lux < 300 has been sent

void loop() {
  float lux = GY30.readLightLevel(); // Read the light level from the sensor
  Serial.println(lux); // Print the data to the serial monitor
  delay(1000); // Pause for a second before repeating the sensor poll

  // Create query string here after lux is declared
  String queryString = "?value1=" + String(lux) + "&value2=" + String(lux);

  if (lux >= 100.00) { 
    if (!aboveThresholdNotified) { 
      aboveThresholdNotified = true; 
      belowThresholdNotified = false;

      if (client.connected()) {
        client.stop();
        Serial.println("Disconnected from server");
      }

      if (client.connect(HOST_NAME, 80)) {
        Serial.println("Connected to server");
      } else {
        Serial.println("Connection failed");
      }

      // Send HTTP request
      client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println();
    }
  } else { 
    if (!belowThresholdNotified) { 
      belowThresholdNotified = true; 
      aboveThresholdNotified = false;

      if (client.connected()) {
        client.stop();
        Serial.println("Disconnected from server");
      }

      if (client.connect(HOST_NAME1, 80)) {
        Serial.println("Connected to server");
      } else {
        Serial.println("Connection failed");
      }

      // Send HTTP request
      client.println("GET " + PATH_NAME1 + queryString + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME1));
      client.println("Connection: close");
      client.println();
      
      delay(1000);
    }
  }
}
