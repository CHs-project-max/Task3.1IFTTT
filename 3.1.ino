#include <WiFiNINA.h>
#include "secrets.h"
#include <PubSubClient.h>  // MQTT library
#include <BH1750FVI.h>

// WiFi Credentials


#define MQTT_SERVER "ee35b2b65e774039ae3e9615543a508b.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/motion"





BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);


// Use SSL Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);


// Function to Connect to MQTT Broker
void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
           Serial.println("Connected to MQTT Broker!");
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }
}


void setup() {
   Serial.begin(115200);
   LightSensor.begin();


   // Connect to WiFi
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   Serial.println("Connected to WiFi");


   // Set MQTT Server
   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
   // Connect to MQTT
   connectMQTT();
}


void loop() {
   if (!mqttClient.connected()) {
       connectMQTT();
   }// Keep MQTT connection alive

  uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.println(lux);
  delay(250);

   if(lux >= 1000) {
       Serial.println("Sunlight detected!");
       mqttClient.publish(MQTT_TOPIC, "Sunlight detected");
       while(lux >= 1000){
      lux = LightSensor.GetLightIntensity();
      delay(10000);
       }
   } else {
       Serial.println("No Sunlight");
       mqttClient.publish(MQTT_TOPIC, "No Sunlight detected");
       while(lux < 1000){
      lux = LightSensor.GetLightIntensity();
       delay(10000);
       }
   }
   delay(5000); // Send data every 5 seconds
}

