
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <strings.h>
#include "secrets.h"
#include "web_utils.h"

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// extern uint8_t * pCallback;
// extern size_t lenCallback;
uint8_t * pCallback;
size_t lenCallback;


// void(WebUtils::*p_recvMsg) (uint8_t *data, size_t len) const = NULL;
// p_recvMsg = &WebUtils::recvMsg;


void WebUtils::recvMsg(uint8_t *data, size_t len){
  this->println("Received Data...");
  // web_utils.CALLBACK_MSG = "";
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  pCallback = data;
  lenCallback = len;
  this->println(d);
}

void WebUtils::setup(void){

  this->connectWifi();

  #ifdef OTA
  this->connectOTAServer();
  #endif

  this->connectWebSerialServer();

  timeClient.begin();

}

bool WebUtils::connectWifi(void){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  //WiFi.setAutoReconnect


  Serial.println("Connecting to Wi-Fi");

  uint32_t wifi_init_timer = millis();
  uint32_t wifi_timer = millis();
  while ((WiFi.status() != WL_CONNECTED) && ((wifi_timer - wifi_init_timer) < 10000)){
    wifi_timer = millis();
  }

  if (WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to Wi-Fi");
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else{
    Serial.println("Failed to connect...");
  }


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is a sample response.");
  });

  return true;

}

void WebUtils::connectOTAServer(void){
  AsyncElegantOTA.begin(&server);
}

void WebUtils::connectWebSerialServer(void)
{
  this->begin(&server);
    /* Attach Message Callback */
  // p_recvMsg = &WebUtils::recvMsg;

  // this->msgCallback(*this.*p_recvMsg);
  // this->setCallbackVals();
  server.begin();
  Serial.println("HTTP server started");
}


bool WebUtils::findTargetNetwork(char *target_network)
{
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    return false;
  }

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {

    // if(strcmp(target_network,WiFi.SSID(thisNet)) == 0)
    if(String(target_network).equals(WiFi.SSID(thisNet))){
      Serial.println("Target network found");
      return true;
    }
  }
  return false;
}

//TODO: add reconnect method when wifi drops

// void WebUtils::setCallbackVals(void){
//   this->pCallback = pCallback;
//   this->lenCallback = lenCallback;
// }

void WebUtils::getCurrentTime(void)
{
  hour = timeClient.getHours();
  seconds = timeClient.getSeconds();
  minute = timeClient.getMinutes();
}


void WebUtils::getCurrentDate(void){
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  day = ptm->tm_mday;
  month = ptm->tm_mon+1;
  year = ptm->tm_year+1900-2000;
}


void WebUtils::updateTimestamp(void)
{
  timeClient.update();
  this->getCurrentDate();
  this->getCurrentTime();
  sprintf(this->timestamp, "%02d-%02d-%02d %02d:%02d:%02d", \
          this->year, this->month, this->day, \
          this->hour, this->minute, this->seconds);
}

int WebUtils::getSignalStrength(void)
{
  return WiFi.RSSI();
}

bool WebUtils::wifiConnected(void)
{
  return WiFi.status() == WL_CONNECTED;
}

