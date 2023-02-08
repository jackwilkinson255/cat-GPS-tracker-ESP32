



enum PARSESTATEVAL
{
	LOOKING_FOR_PMTK001,
	LOOKING_FOR_PMTKLOX0,
	READ_IN_DATA,
	READ_COMPLETED,
	READ_ERROR,
	BUFFER_DONE,
};

struct locusValues
{
	unsigned char locusString[16];
	unsigned long unixTime;
    char timestamp[18];
	unsigned long latInLong;
	unsigned long lonInLong;
	float latitude;
	float longitude;
	short height;
	unsigned char checksum;
	char fix;
    char inLineBuffer[256];
	PARSESTATEVAL parseState = LOOKING_FOR_PMTK001;
	int sentenceNo;
};

class ParseLOCUS
{
    public:
        int parseLocusString(void);
        void parseLocusState(void);
		bool newNMEAreceived(char c);
		bool NMEAValid(void);
        struct locusValues locusArray;
		char lastNMEA[256];

    private:
        unsigned char convHexNibble(unsigned char nibbleVal);
        void formatTime(void);
        void checkChecksum(void);
        void parseLocusValues(void);

};