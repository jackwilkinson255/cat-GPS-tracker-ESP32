#include <Arduino.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "gps_utils.h"
#include "parse_locus.h"

static const uint32_t GPSBaud = 9600;

ParseLOCUS locus;


void GPS_Utils::setup(void)
{
    this->begin(GPSBaud);
    this->setupCommon();
}


void GPS_Utils::setup(mode modeType)
{
    this->begin(GPSBaud);
    this->gps_mode = modeType;

    switch (this->gps_mode){
        case view:
            this->setupCommon();
            break;
        case logging:
            this->setupCommon();
            break;
        case erase:
            this->setupEraseData();
            break;
        case dump:
            this->setupDumpData();
            break;
    }
}


void GPS_Utils::setupCommon(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    this->sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);
    this->sendCommand(PGCMD_ANTENNA);
}


void GPS_Utils::setupDumpData(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);

    while (this->available())
        this->read();

    delay(1000);

    this->sendCommand(PMTK_LOCUS_DUMPDATA);
}


void GPS_Utils::setupEraseData(void)
{
    this->sendCommand(PMTK_SET_NMEA_OUTPUT_OFF);
    delay(500);
    Serial.println("Erasing logger data...");
    this->sendCommand(PMTK_LOCUS_ERASE_FLASH);
    delay(1000);

    Serial.println("Erased...");
    while(1){}

}



void GPS_Utils::loop(void)
{
    switch (this->gps_mode){
        case view:
            this->getData();
            this->viewLoop();
            break;
        case logging:
            this->getData();
            this->loggerLoop();
            break;
        case erase:
            // do nothing
            break;
        case dump:
            this->dumpDataLoop();
            break;
    }
}


void GPS_Utils::loggerLoop(void)
{
    // if (!loggerStarted) // skip everything if logger has begun
    // {
        if ((p_gpsserial->available()) > 0){
            char c = this->read();

            if (this->newNMEAreceived()) {
                if (!this->parse(this->lastNMEA()))
                    return;
            }

            // if (this->fix)
            //     this->startLogger();
        }
    // }

    //TODO stop logger
}


void GPS_Utils::viewLoop(void)
{
    if ((p_gpsserial->available()) > 0){
        char c = this->read();

        if (this->newNMEAreceived()) {
            if (!this->parse(this->lastNMEA()))
                return;
        }
    }
}


void GPS_Utils::startLogger(void)
{

    if (!loggerStarted){
        Serial.print("Starting logger...  ");
        while (!this->LOCUS_StartLogger())
            this->LOCUS_StartLogger();
        loggerStarted = true;
        Serial.println("STARTED!");
    } else
        Serial.println("Logger already running...");

}


void GPS_Utils::stopLogger(void)
{
    if (loggerStarted){
        Serial.print("Stopping logger...  ");
        while (!this->LOCUS_StopLogger())
            this->LOCUS_StopLogger();
        loggerStarted = false;
        Serial.println("STOPPED!");
    } else
        Serial.println("Logger not running...");
}


bool GPS_Utils::getLoggerStatus(void)
{
    return loggerStarted;
}


void GPS_Utils::dumpDataLoop(void)
{
    int sentenceNum = 0;
    Serial.println("===============================================================================================================================================================================");
    Serial.println("waypoint,latitude,longitude,timestamp,nmea_sentence");
    while (locus.locusArray.parseState != BUFFER_DONE || locus.locusArray.parseState != READ_COMPLETED)
    {
        if ((p_gpsserial->available()) > 0)
        {
            char c = p_gpsserial->read();

            if (locus.newNMEAreceived(c))
            {

                locus.parseLocusState();

                if (locus.locusArray.parseState == READ_IN_DATA)
                {
                    locus.parseLocusString();

                    if ((!isinf(locus.locusArray.latitude)) && (!isinf(locus.locusArray.longitude)) &&\
                        (locus.locusArray.latitude != 0.0) && (locus.locusArray.longitude != 0.0))
                    {
                        Serial.printf("point_%i,%.8f,%.8f,%s,\"%s\"\n", ++sentenceNum, locus.locusArray.latitude, locus.locusArray.longitude, locus.locusArray.timestamp, locus.locusArray.inLineBuffer);
                    }
                }


            }

            if (locus.locusArray.parseState == READ_COMPLETED)
                break;

        }
    }
    Serial.println("Buffer complete...");
    while(1){}
}


void GPS_Utils::getData()
{
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
    if (sign == 'X'){
        if (loc < 0){
            sign = '-';
            loc *= -1;
        } else {
            sign = '+';
        }
        loc *= 100;
    }

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

void GPS_Utils::getLocusInfo(void)
{
    static u_int32_t timer;

    // if (millis() - timer > 2000)
    // {
        while(1){
        if(this->LOCUS_ReadStatus())
            break;
        }

    Serial.println("GPS Configuration Settings:");
    Serial.print("serial: ");
    Serial.println(LOCUS_serial);
    Serial.print("type: ");
    Serial.println(LOCUS_type);
    Serial.print("mode: ");
    Serial.println(LOCUS_mode);
    Serial.print("config: ");
    Serial.println(LOCUS_config);
    Serial.print("interval: ");
    Serial.println(LOCUS_interval);
    Serial.print("distance: ");
    Serial.println(LOCUS_distance);
    Serial.print("speed: ");
    Serial.println(LOCUS_speed);
    Serial.print("status: ");
    Serial.println(LOCUS_status);
    Serial.print("records: ");
    Serial.println(LOCUS_records);
    Serial.print("percent: ");
    Serial.println(LOCUS_percent);
    Serial.println();

    //     timer = millis();
    // }
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

