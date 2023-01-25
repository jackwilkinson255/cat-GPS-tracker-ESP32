#include <Arduino.h>
#include <ArduinoJson.h>
#include "web_utils.h"
#include "gps_utils.h"
#include "aws_iot.h"


#define PRINT_TIMER(start_time) (millis()-start_time) >= 2000
#define AWS_TIMER(start_time) (millis()-start_time) >= 1000

WebUtils web;
AWSIOT aws;
#ifdef FIREBEETLE_BOARD
HardwareSerial *pSerial = &Serial2;
#elif QT_BOARD
HardwareSerial *pSerial = &Serial1;
#endif
GPS_Utils gps(pSerial);

// Timers
u_int32_t awsTime = 0;
u_int32_t printTime = 0;

StaticJsonDocument<200> doc;

char timestamp[24];

void printData(void);


void setup()
{
  #if DEBUG_PORT == Serial
  DEBUG_PORT.begin(115200);
  DEBUG_PORT.println("Starting up...");
  #endif

  gps.setup(dump);
  delay(1000);
  web.setup();
  delay(1000);
  aws.setup();
}


void loop()
{
    web.loop();
    gps.loop();
    aws.loop();

    if(AWS_TIMER(awsTime)){
      doc["date"] = gps_s.date;
      doc["gps_time"] = gps_s.gps_time;
      doc["lat"] = gps_s.lat;
      doc["lng"] = gps_s.lng;
      aws.publishMessage(doc);
      awsTime = millis();
    }

    if(PRINT_TIMER(printTime)){
      printData();
      printTime = millis();
    }
}


void printData(void)
{
  DEBUG_PORT.print("\n\ntimestamp: ");
  DEBUG_PORT.print(gps.timestamp);
  DEBUG_PORT.print("\t location: ");
  DEBUG_PORT.print(gps.location);
  DEBUG_PORT.print("\t fix: ");
  DEBUG_PORT.print(gps.fix);
  DEBUG_PORT.print("\t hour: ");
  DEBUG_PORT.print(gps.hour);
  DEBUG_PORT.println();
}