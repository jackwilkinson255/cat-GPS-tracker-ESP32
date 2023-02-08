#include <Arduino.h>
#include <Adafruit_GPS.h>


#ifdef TESTING
   #define PRIVATE_TESTING public
#else
    #define PRIVATE_TESTING private
#endif
#define PMTK_LOCUS_DUMPDATA "$PMTK622,1*29"

enum mode {
    view,
    logging,
    erase,
    dump
};


class GPS_Utils: public Adafruit_GPS
{
    public:
        GPS_Utils(HardwareSerial *GPSSerial): Adafruit_GPS(GPSSerial), p_gpsserial(GPSSerial){};

        typedef struct gps_struct
        {
            char gps_time[9];// = "00:00:00";
            char date[9];// = "00-00-00";
            char lat[9];// = "0000.00N";
            char lon[10];// = "00000.00W";
        } gps_struct;



        char timestamp[18];
        char location[22];


        void setup(void);
        void setup(mode modeType);
        void loop(void);
        void getData();
        void getLocusInfo();
        bool getLoggerStatus(void);
        void startLogger(void);
        void stopLogger(void);
        mode gps_mode;

    PRIVATE_TESTING:
        void getLocation(void);
        void getTimestamp(void);
        void getFix(void);
        void getOtherInfo(void);
        void setupCommon(void);
        void loggerLoop(void);
        void viewLoop(void);
        void setupDumpData(void);
        void setupEraseData(void);
        void dumpDataLoop(void);
        void formatDecimalMins(char sign, float loc, char *formatted);
        bool loggerStarted = false;
        HardwareSerial *p_gpsserial;


};