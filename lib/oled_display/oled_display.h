// #include <string.h>

class OLEDDisplay
{
    // private:

        /* data */
    public:
        // void printMessage(char *msg);

        void setup(void);
        void print(void);
        void updateDisplay(char *timestamp, char *location, bool fix, char *battery, char *wifi_strength);

        // LCDDisplay(/* args */);
        // ~LCDDisplay();
};

// LCDDisplay::LCDDisplay(/* args */)
// {
// }

// LCDDisplay::~LCDDisplay()
// {
// }


