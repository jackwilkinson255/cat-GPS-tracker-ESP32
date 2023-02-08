#include <Arduino.h>
#include <WebSerial.h>
#include <ArduinoJson.h>
#include <Battery.h>
#include "web_utils.h"
#include "gps_utils.h"
#include "aws_iot.h"
#include "oled_display.h"
#include "misc_utils.h"

#define WS_TIMER(start_time) (millis()-start_time) >= 2000
#define PRINT_TIMER(start_time) (millis()-start_time) >= 1000
#define GPS_TIMER(start_time) (millis()-start_time) >= 1000
#define AWS_TIMER(start_time) (millis()-start_time) >= 1000
#define BATT_PIN A2

WebUtils web;
AWSIOT aws;
OLEDDisplay oled;
DeepSleep ds;
// NeoPix neoPixel(1);
Battery battery(3700, 4200, BATT_PIN);
#ifdef FIREBEETLE
HardwareSerial *pSerial = &Serial2;
#elif QTPY
HardwareSerial *pSerial = &Serial1;
#endif
GPS_Utils gps(pSerial);

// Timers
u_int32_t count = 0;
u_int32_t gpsTime = 0;
u_int32_t wsTime = 0;
u_int32_t awsTime = 0;
u_int32_t printTime = 0;

StaticJsonDocument<200> doc;

enum cat_location {
  inside,
  outside
} cat_loc;


// Function prototypes
cat_location getCatLoc(void);
void printData(void);
void displayLEDdata(void);
void printOnce(char printChar[]);
float readBattery(void);

mode gps_mode = GPS_MODE;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting up...");
  if (GPS_MODE == logging){
    delay(1000);
    Serial.println("LOGGING MODE, DISCONNECT");
    delay(5000);
    // neoPixel.off();
  } else if (GPS_MODE == view)
    oled.setup();
  battery.begin(3300, 1.0, &sigmoidal);

  web.setup();
  gps.setup(GPS_MODE);
  // aws.setup();
}



void loop()
{
    gps.loop();

    if (cat_loc == outside)
      web.connectWifi();

    if (GPS_MODE == logging)
    {
      cat_loc = getCatLoc();

      switch (cat_loc)
      {
      case outside:
        gps.startLogger();
        Serial.println("Logger started, going to sleep");
        ds.enterDeepSleep(5*60);
        break;

      case inside:
        gps.stopLogger();
        Serial.println("Logger stopped, going to attempt upload");
        ds.enterDeepSleep(5*60);
        break;

      default:
        break;
      }
    }

      if(PRINT_TIMER(printTime)){
        printData();
        displayLEDdata();
        printTime = millis();
      }

}

cat_location getCatLoc(void)
{
  if ((gps.fix && !web.wifiConnected()) || (!gps.fix && !web.wifiConnected()))// if fix and no wifi
    return outside;
  else if ((!gps.fix && web.wifiConnected()) || (gps.fix && web.wifiConnected()))
    return inside;
}

float readBattery(void)
{
  return analogRead(A2);
}


void printData(void)
{
  char wifi_strength[64];
  char battery_str[64];
  sprintf(wifi_strength, "%idBm  Status: %i", web.getSignalStrength(), web.wifiConnected());
  sprintf(battery_str, "%imV  %i%", battery.voltage(), battery.level());

  DEBUG_ESP32.printf("timestamp: %s\t location: %s\t fix: %d\t battery: %s\t wifi: %s\n", gps.timestamp, gps.location, gps.fix, battery_str, wifi_strength);
}

void printOnce(char printChar[]){
  static bool printed;
  if (!printed){
    Serial.println(printChar);
    printed = true;
  }

}

void displayLEDdata(void)
{
  char wifi_strength[64];
  char battery_str[64];
  sprintf(wifi_strength, "%idBm Status:%i", web.getSignalStrength(), web.wifiConnected());
  sprintf(battery_str, "%imV  %i\%", battery.voltage(), battery.level());
  oled.updateDisplay(gps.timestamp, gps.location, gps.fix, battery_str, wifi_strength);
}

void printPeriodic(char *msg, int freq_secs)
{
  static int curr_time;
  if ((millis()-curr_time) > freq_secs*1000){
    Serial.println(msg);
    curr_time = millis();
  }
}

