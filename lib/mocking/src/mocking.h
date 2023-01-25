#include <stdio.h>
#include <stdint.h>


#ifdef TESTING
   #define PRIVATE_TESTING public
#else
    #define PRIVATE_TESTING private
#endif

class GPS_Utils_Mock
{

    public:
        uint8_t hour;
        uint8_t minute;
        uint8_t seconds;
        uint8_t year;
        uint8_t month;
        uint8_t day;

        char lat;
        char lon;
        float latitude;
        float longitude;

        bool fix = true;
        uint8_t fixquality = 2;

        char timestamp[18];
        char location[22];



    PRIVATE_TESTING:
        void getTimestamp(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t seconds);
        void getTimestamp(void);
        void getDate(void);
        void getLocation(void);
        void getLocation(float latitude, char lat, float longitude, char lon);
        void formatDecimalMins(char sign, float loc, char *formatted);
        void getFix(void);
        void getOtherInfo(void);

};