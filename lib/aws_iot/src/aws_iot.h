#include <Arduino.h>
#include <ArduinoJson.h>

class AWSIOT
{
    private:
        void messageHandler(String &topic, String &payload);

    public:
        void setup(void);
        void publishMessage(const JsonDocument& _doc);
        void loop(void);
};