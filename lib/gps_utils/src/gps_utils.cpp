#include <Arduino.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "gps_utils.h"

static const uint32_t GPSBaud = 9600;


void GPS_Utils::setup(void)
{
    this->begin(GPSBaud);
    this->setupLogger();
}


void GPS_Utils::setup(mode modeType)
{
    this->begin(GPSBaud);
    this->gps_mode = modeType;

    switch (this->gps_mode){
        case view:
            this->setupLogger();
            break;
        case logging:
            this->setupLogger();
            break;
        case erase:
            this->setupEraseData();
            break;
        case dump:
            this->setupDumpData();
            break;
    }
}


void GPS_Utils::setupLogger(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    this->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    this->sendCommand(PGCMD_ANTENNA);
}


void GPS_Utils::setupDumpData(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);

    while (p_gpsserial->available())
        p_gpsserial->read();

    delay(1000);
    this->sendCommand(PMTK_LOCUS_DUMPDATA);
    Serial.println("----------------------------------------------------");
}


void GPS_Utils::setupEraseData(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
    Serial.println("Erasing logger data...");
    this->sendCommand(PMTK_LOCUS_ERASE_FLASH);
    Serial.println("Erased");
}



void GPS_Utils::loop(void)
{
    switch (this->gps_mode){
        case view:
            this->getData();
            break;
        case logging:
            this->getData();
            this->loggerLoop();
            break;
        case erase:
            // do nothing
            break;
        case dump:
            this->getData();
            this->dumpDataLoop();
            break;
    }
}


void GPS_Utils::loggerLoop(void)
{
    char c = this->read();

    if (this->newNMEAreceived()) {
        if (!this->parse(this->lastNMEA()))
            return;
    }

    if (this->fix)
        this->startLogger();
}


void GPS_Utils::startLogger(void)
{
    static bool loggerStarted;

    if (!loggerStarted){
        if (this->LOCUS_StartLogger()){
            Serial.print("\nSTARTING LOGGING....");
            loggerStarted = true;
            Serial.println("\t Logger started!");
        }
        else
            Serial.println(" no response");
    }
}


void GPS_Utils::dumpDataLoop(void)
{
//   if (Serial.available()) {
//     char c = Serial.read();
//     p_gpsserial->write(c);
//   }
    char c = this->read();

    if (this->newNMEAreceived()) {
        // if (!this->parse(this->lastNMEA()))
        Serial.print(this->lastNMEA());
        this->parse(this->lastNMEA());

            // return;
    }




//   if (p_gpsserial->available()) {
//     char c = p_gpsserial->read();
//     this->parse(this->lastNMEA());
//     Serial.write(c);
//   }
}


// void GPS_Utils::dumpDataLoop(void)
// {
//   if (Serial.available()) {
//     char c = Serial.read();
//     p_gpsserial->write(c);
//   }
//   if (p_gpsserial->available()) {
//     char c = p_gpsserial->read();
//     Serial.write(c);
//   }
// }


void GPS_Utils::getData()
{
    //TODO: Also retreive speed, angle, satellites, antenna status
    this->getTimestamp();
    if(this->fix){
        this->getLocation();
    }

}


void GPS_Utils::getTimestamp(void)
{
    sprintf(this->timestamp, "%02d-%02d-%02d %02d:%02d:%02d", this->year, this->month, this->day, this->hour, this->minute, this->seconds);
}



void GPS_Utils::getLocation(void)
{
    /*
    Latitude: 2851.77  Lat: N   --> +28 51.77’
    Longitude: 1351.52  Lng: W  --> -13 51.52’
    N and E are positive. S and W are negative
    */
    char lat_formatted[11];
    char lon_formatted[12];

    char lat_sign = (this->lat == 'N') ? '+' : '-';
    char lon_sign = (this->lon == 'E') ? '+' : '-';

    this->formatDecimalMins(lat_sign, this->latitude, lat_formatted);
    this->formatDecimalMins(lon_sign, this->longitude, lon_formatted);

    sprintf(this->location, "%s, %s", lat_formatted, lon_formatted);
}


void GPS_Utils::formatDecimalMins(char sign, float loc, char *formatted)
{
    char loc_dm[9];
    if (loc < 1000.0){
        sprintf(loc_dm, "%6.2f", loc);
        sprintf(formatted, "%c%c %c%c.%c%c'", sign, loc_dm[0], loc_dm[1], loc_dm[2], loc_dm[4], loc_dm[5]);
    }
    else if(loc < 10000.0){
        sprintf(loc_dm, "%7.2f", loc);
        sprintf(formatted, "%c%c%c %c%c.%c%c'", sign, loc_dm[0], loc_dm[1], loc_dm[2], loc_dm[3], loc_dm[5], loc_dm[6]);
    }
    else {
        sprintf(loc_dm, "%8.2f", loc);
        sprintf(formatted, "%c%c%c%c %c%c.%c%c'", sign, loc_dm[0], loc_dm[1], loc_dm[2], loc_dm[3], loc_dm[4], loc_dm[6], loc_dm[7]);
    }
}


void GPS_Utils::getFix(void)
{
    Serial.print("Fix: "); Serial.print((int)this->fix);
    Serial.print(" quality: "); Serial.println((int)this->fixquality);
}


void GPS_Utils::getOtherInfo(void)
{
    Serial.print("Satellites: "); Serial.println((int)this->satellites);
    Serial.print("Antenna status: "); Serial.println((int)this->antenna);
}

