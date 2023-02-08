#include <Arduino.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "aws_iot.h"
#include "secrets.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);




void AWSIOT::messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
}


void AWSIOT::setup()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  // client.onMessage(messageHandler);
  client.onMessage(
    [this](String &topic, String &payload){
        this->messageHandler(topic, payload);
    });


  /*
  wifi.onEvent(
  [this](WiFiEvent_t event, system_event_info_t info) {
    this->WiFiEvent(event, info);
  });
  */

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}



// void AWSIOT::messageHandler(String &topic, String &payload) {
//   Serial.println("incoming: " + topic + " - " + payload);
//   StaticJsonDocument<200> doc;
//   deserializeJson(doc, payload);
//   const char* message = doc["message"];
// }

void AWSIOT::publishMessage(const JsonDocument& doc)
{
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void AWSIOT::loop()
{
    client.loop();
}
