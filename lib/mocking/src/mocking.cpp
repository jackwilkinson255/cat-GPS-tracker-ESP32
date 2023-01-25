#include "mocking.h"

void GPS_Utils_Mock::getLocation(void)
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

void GPS_Utils_Mock::getLocation(float latitude, char lat, float longitude, char lon)
{
    /*
    Latitude: 2851.77   Lat: N
    Longitude: 1351.52   lon: W
    +28 51.77’,-13 51.52’
    N and E are positive
    S and W are negative
    */
    char lat_formatted[11];
    char lon_formatted[12];

    char lat_sign = (lat == 'N') ? '+' : '-';
    char lon_sign = (lon == 'E') ? '+' : '-';

    this->formatDecimalMins(lat_sign, latitude, lat_formatted);
    this->formatDecimalMins(lon_sign, longitude, lon_formatted);

    sprintf(this->location, "%s, %s", lat_formatted, lon_formatted);
}

void GPS_Utils_Mock::formatDecimalMins(char sign, float loc, char *formatted)
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


void GPS_Utils_Mock::getTimestamp(void)
{
    sprintf(this->timestamp, "%02d-%02d-%02d %02d:%02d:%02d", this->year, this->month, this->day, this->hour, this->minute, this->seconds);
}


void GPS_Utils_Mock::getTimestamp(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t seconds)
{
    sprintf(this->timestamp, "%02d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, seconds);
}