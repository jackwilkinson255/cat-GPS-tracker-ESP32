#include <Arduino.h>
#include <WebSerial.h>

#ifdef TESTING
   #define PRIVATE_TESTING public
#else
    #define PRIVATE_TESTING private
#endif

class WebUtils: public WebSerialClass
{
    public:
        uint8_t * pCallback;
        size_t lenCallback;
        char timestamp[18];
        void setup(void);
        bool connectWifi(void);
        void connectOTAServer(void);
        void connectWebSerialServer(void);
        bool findTargetNetwork(char *target_network);
        void setCallbackVals(void);
        // String getCallback(void);
        void updateTimestamp(void);
        int getSignalStrength(void);
        bool wifiConnected(void);



    PRIVATE_TESTING:
        u_int8_t year;
        u_int8_t month;
        u_int8_t day;
        u_int8_t hour;
        u_int8_t minute;
        u_int8_t seconds;
        void recvMsg(uint8_t *data, size_t len);
        void getCurrentTime(void);
        void getCurrentDate(void);
        // void(*p_recvMsg) (uint8_t *data, size_t len);
};


