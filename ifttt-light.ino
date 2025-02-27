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

//defines IFTTT event webhook listeners
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/Sunlight-detected-over-terranium/with/key/bWL5vFJUyBZMaaYxHoKZx6"; // change your EVENT-NAME and YOUR-KEY
String queryString = "?value1=57&value2=25";
char   HOST_NAME1[] = "maker.ifttt.com";
String PATH_NAME1   = "/trigger/sun-off-terrarium/with/key/bWL5vFJUyBZMaaYxHoKZx6"; // change your EVENT-NAME and YOUR-KEY


void setup() {
  Serial.begin(9600);

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");

    Wire.begin(); // Initialize the I2C bus for use by the BH1750 library  
  GY30.begin(); // Initialize the sensor object

if (client.connect(HOST_NAME, 80)) {
    // if connected:
    Serial.println("Connected to server");
  }
  else {// if not connected:
    Serial.println("connection failed");
  }

}


bool aboveThresholdNotified = false; // Flag to track if notification for lux > 300 has been sent
bool belowThresholdNotified = false; // Flag to track if notification for lux < 300 has been sent

void loop() {
  float lux = GY30.readLightLevel(); // read the light level from the sensor and store it in a variable
  Serial.println(lux); // print the data to the serial monitor
  delay(1000); // Pause for a second before repeating the sensor poll

  if (lux >= 300.00) { // Send trigger if lux exceeds 300.00
    if (!aboveThresholdNotified) { // Check if notification has not been sent yet
      aboveThresholdNotified = true; // Set the flag to indicate notification sent
      belowThresholdNotified = false; // Reset the flag for below threshold notification

      // Disconnect from the server to allow new trigger
      if (client.connected()) {
        client.stop();
        Serial.println("Disconnected from server");
      }

      // Attempt to reconnect to the server
      if (client.connect(HOST_NAME, 80)) {
        Serial.println("Connected to server");
      } else {
        Serial.println("Connection failed");
      }

      // make a HTTP request for lux > 300 notification:
      // send HTTP header
      client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println(); // end HTTP header
    }
  } else { // lux < 300
    if (!belowThresholdNotified) { // Check if notification has not been sent yet
      belowThresholdNotified = true; // Set the flag to indicate notification sent
      aboveThresholdNotified = false; // Reset the flag for above threshold notification

      // Disconnect from the server to allow new trigger
      if (client.connected()) {
        client.stop();
        Serial.println("Disconnected from server");
      }

      // Attempt to reconnect to the server
      if (client.connect(HOST_NAME, 80)) {
        Serial.println("Connected to server");
      } else {
        Serial.println("Connection failed");
      }

      // make a HTTP request for lux < 300 notification:
      // send HTTP header
      client.println("GET " + PATH_NAME1 + queryString + " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME1));
      client.println("Connection: close");
      client.println(); // end HTTP header
    
    delay(1000);
    
  }
  }
}
    