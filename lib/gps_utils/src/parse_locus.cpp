// parseLOCUS.cpp : Parse the LOCUS data
// Input:
//	The LOCUS string is already parsed to pull out the ASCII nibbles and put the data into a char array.
// Output:
//  The data in readable format to the screen

/*
Copyright (c) 2012 Doug Gilliland

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// 16-byte LOCUS buffer format
// 0-3 = UTC (in UNIX ticks format)
// 4 = FIX flag
// 5-8 = Latitude
// 9-12 = Longitude
// 13-14 = Height
// 15 = Checksum
// Example string = 468C4850 02 B5092042 652D9FC2 E600 00

#include <Arduino.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "parse_locus.h"


/* Sample log ---
--- Log Start ---
$PMTK001,314,3*36
$PMTKLOX,0,43*6E
$PMTKLOX,1,0,0100010A,1F000000,0F000000,0000100A,00000000,00000000,0000007F,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,00FC8C1C,288C4850,02760620,42642D9F,C2B90001,378C4850,027D0720,42532D9F,C2CB0051*23
$PMTKLOX,1,1,468C4850,02B50920,42652D9F,C2E600FD,558C4850,02E30B20,427A2D9F,C2120150,648C4850,02C30D20,42892D9F,C22D018B,738C4850,022A0E20,422E2E9F,C23701C8,828C4850,02880E20,42912E9F,C22D013E,918C4850,02561020,428D2E9F,C22201FE*20
$PMTKLOX,1,2,A08C4850,02D91120,42CA2E9F,C2270103,AF8C4850,02F01120,42C52E9F,C22D0120,BE8C4850,02F01120,42C72E9F,C22E0130,CD8C4850,02FF1120,423D2F9F,C22901B0,DC8C4850,02D01120,427F309F,C22401DE,EB8C4850,023C1120,42EC319F,C2210192*27
$PMTKLOX,1,3,FA8C4850,02AA1020,42A8329F,C2200152,098D4850,02930E20,423B329F,C21C0128,188D4850,02E80D20,4285329F,C21501F6,278D4850,02BF0D20,42F5329F,C21401EF,368D4850,02B70D20,4208339F,C2160108,458D4850,021E0D20,421D339F,C21C01CD*22
$PMTKLOX,1,4,548D4850,02F10A20,42EF329F,C22201F9,638D4850,02E30820,42E2339F,C23201C2,728D4850,02530620,428A349F,C23A010A,818D4850,02680320,4202359F,C2350141,908D4850,028D0120,42A8359F,C23C0114,9F8D4850,02B50020,42F3359F,C23F017A*27
$PMTKLOX,1,5,AE8D4850,0462FE1F,42DF369F,C244010F,BD8D4850,04FBFB1F,4235389F,C2330113,CC8D4850,0490FA1F,42F8399F,C23101C6,DB8D4850,04BCF91F,42E13B9F,C23501E1,EA8D4850,04E8F81F,42C83D9F,C23D01A2,F98D4850,04EBF71F,42973F9F,C24A0197*22
$PMTKLOX,1,6,088E4850,04F2F51F,423F419F,C25601B4,178E4850,04FDF21F,426C429F,C26101C4,268E4850,0485EF1F,42E1429F,C257012B,358E4850,0475EC1F,4216449F,C266010B,448E4850,0479E91F,4292459F,C27001E0,538E4850,047FE61F,420C479F,C27A0168*5C
$PMTKLOX,1,7,628E4850,049DE31F,427A489F,C281013C,718E4850,04F7E01F,42CB499F,C28101F6,808E4850,0478DE1F,420C4B9F,C27D018F,8F8E48FFFFFFF,FFFFFFFF,FFFFFFFF*6E
$PMTKLOX,2*47
$PMTK001,622,3*36
*/


// bool ParseLOCUS::parseLocusString(void)
// {

// }

bool ParseLOCUS::newNMEAreceived(char c)
{
	// static char newNMEALine[256];
	static int charIdx;


	if (c == '\r'){
		return false;
	}

	if (c == '$'){
		if (locusArray.sentenceNo > 0){
			lastNMEA[++charIdx] = 0;

			strcpy(locusArray.inLineBuffer, lastNMEA);
			locusArray.inLineBuffer[strcspn(locusArray.inLineBuffer, "\n")] = 0;
		}
		charIdx = 0;
		lastNMEA[charIdx] = c;
		++charIdx;
		++locusArray.sentenceNo;
		return true;

	} else {
		lastNMEA[charIdx] = c;
		++charIdx;
		return false;
	}


}

bool ParseLOCUS::NMEAValid(void)
{
	int linePtr = 9;
	// $PMTKLOX,1,0,0100010A,1F000000,0F000000,0000100A,00000000,00000000,0000007F,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,00FC8C1C,288C4850,02760620,42642D9F,C2B90001,378C4850,027D0720,42532D9F,C2CB0051*23
	int charVal = 0;
	while(charVal < 2){
		while (locusArray.inLineBuffer[linePtr] != ','){
			if (locusArray.inLineBuffer[linePtr] == '*')
				return false;
			linePtr++;
		}
		linePtr++;
		charVal++;
	}

	charVal = 0;
	while(charVal < 5){

		while (locusArray.inLineBuffer[linePtr] != ','){
			if (locusArray.inLineBuffer[linePtr] != 'F'){
				return true;
			} else if (locusArray.inLineBuffer[linePtr] == '*'){
				return false;
			}
			linePtr++;
		}
		charVal++;
	}
	return false;
}



void ParseLOCUS::parseLocusState(void)
{

	switch (locusArray.parseState)
	{
	case LOOKING_FOR_PMTK001:
		if (strcmp(locusArray.inLineBuffer,"$PMTK001,314,3*36") == 0)
			locusArray.parseState = LOOKING_FOR_PMTKLOX0;

	case LOOKING_FOR_PMTKLOX0:
		char tempString[256];
		strcpy(tempString,locusArray.inLineBuffer);
		tempString[11] = 0;
		if (strcmp(tempString,"$PMTKLOX,0,") == 0)
			locusArray.parseState = READ_IN_DATA;
		break;

	case READ_IN_DATA:
		if ((strcmp(locusArray.inLineBuffer,"$PMTKLOX,2*47") == 0))
			locusArray.parseState = READ_COMPLETED;

		else if ((strcmp(locusArray.inLineBuffer,"$PMTK001,622,3*36") == 0))
			locusArray.parseState = READ_COMPLETED;

		//$PMTKLOX,
		char tempString2[256];
		strcpy(tempString2,locusArray.inLineBuffer);
		tempString2[9] = 0;
		if (strcmp(tempString2,"$PMTKLOX,") != 0)
		{
			locusArray.parseState = READ_ERROR;
		}
		if (parseLocusString() != 0)
		{
			locusArray.parseState = READ_COMPLETED;
		}
		break;
	case READ_COMPLETED:
		if ((strcmp(locusArray.inLineBuffer,"$PMTK001,622,3*36") == 0))
			locusArray.parseState = BUFFER_DONE;
		else if ((strcmp(locusArray.inLineBuffer,"$PMTKLOX,2*47") == 0))
			locusArray.parseState = BUFFER_DONE;
		break;
	}
}


int ParseLOCUS::parseLocusString(void)
{
	unsigned char charVal, charValUpperLower;
	int whichCharToInsert = 0;
	int linePtr = 9;
	// $PMTKLOX,1,0,0100010A,1F000000,0F000000,0000100A,00000000,00000000,0000007F,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,FFFFFFFF,00FC8C1C,288C4850,02760620,42642D9F,C2B90001,378C4850,027D0720,42532D9F,C2CB0051*23
	while (locusArray.inLineBuffer[linePtr] != ',')
		linePtr++;
	linePtr++;
	while (locusArray.inLineBuffer[linePtr] != ',')
		linePtr++;
	linePtr++;
	while (1)
	{
		for (int loopCount2 = 0; loopCount2 < 4; loopCount2++)
		{
			charVal = convHexNibble(locusArray.inLineBuffer[linePtr++]);
			if (charVal == -1)
			{
				Serial.println("Character conversion error on line with contents");
				return(-2);
			}
			charValUpperLower = 0;
			charValUpperLower = charVal << 4;
			charValUpperLower &= 0xf0;
			charVal = convHexNibble(locusArray.inLineBuffer[linePtr++]);
			if (charVal == -1)
			{
				Serial.println("Character conversion error on line with contents");
				return(0);
			}
			charVal &= 0xf;
			charValUpperLower |= (unsigned char) charVal;
			locusArray.locusString[whichCharToInsert++] = charValUpperLower;
			if (whichCharToInsert == 16)
			{
				parseLocusValues();
				formatTime();
				whichCharToInsert = 0;
			}
		}
		if ((locusArray.inLineBuffer[linePtr] != '*') && (locusArray.inLineBuffer[linePtr] != ','))
		{

			// printf("Missed a comma/star?\n");
			// printf("Parsing line = %s\n",locusArray.inLineBuffer);
			// printf("At column = %d\n",linePtr+1);
			getchar();
			return(-1);
		}
		if (locusArray.inLineBuffer[linePtr] == '*')
			return(0);
		linePtr++;
	}
}


unsigned char ParseLOCUS::convHexNibble(unsigned char nibbleVal)
{
	if ((nibbleVal >= '0') && (nibbleVal <= '9'))
		return(nibbleVal - '0');
	else if ((nibbleVal >= 'A') && (nibbleVal <= 'F'))
		return(nibbleVal - 'A' + 10);
	return(-1);
}


void ParseLOCUS::formatTime(void)
{
	time_t now = locusArray.unixTime;
	strftime(locusArray.timestamp, sizeof locusArray.timestamp, "%y-%m-%d %H:%M:%S", localtime(&now));
}


void ParseLOCUS::checkChecksum(void)
{
	unsigned char csum = 0;
	int loopIndex;
	for (loopIndex = 0; loopIndex < 16; loopIndex++)
	{
		csum ^= locusArray.locusString[loopIndex];
	}
	if (csum == 0)
		printf("Checksum OK\n");
	else
		printf("Checksum failed = %d\n",csum);
}


void ParseLOCUS::parseLocusValues(void)
{
	// Calculate UTC
	locusArray.unixTime = locusArray.locusString[3];
	locusArray.unixTime &= 0xff;
	locusArray.unixTime <<= 8;
	locusArray.unixTime |= locusArray.locusString[2];
	locusArray.unixTime &= 0xffff;
	locusArray.unixTime <<= 8;
	locusArray.unixTime |= locusArray.locusString[1];
	locusArray.unixTime &= 0xffffff;
	locusArray.unixTime <<= 8;
	locusArray.unixTime |= locusArray.locusString[0];

	locusArray.fix = locusArray.locusString[4];

	locusArray.height = locusArray.locusString[14];
	locusArray.height &= 0xff;
	locusArray.height <<= 8;
	locusArray.height |= locusArray.locusString[13];

	locusArray.latInLong = 0;
	locusArray.latInLong = locusArray.locusString[8]<<24;
	locusArray.latInLong |= (locusArray.locusString[7] << 16);
	locusArray.latInLong |= (locusArray.locusString[6] << 8);
	locusArray.latInLong |= locusArray.locusString[5];
	float exponent = (float)((locusArray.latInLong >> 23) & 0xff);
	exponent -= 127.0;
	exponent = pow(2,exponent);
	float mantissa;
	mantissa = (float)(locusArray.latInLong & 0x7fffff);
	mantissa = 1.0 + (mantissa/8388607.0);
	locusArray.latitude = mantissa * exponent;
	if ((locusArray.latInLong & 0x80000000) == 0x80000000)
		locusArray.latitude = -locusArray.latitude;

	locusArray.lonInLong = 0;
	locusArray.lonInLong = locusArray.locusString[12] << 24;
	locusArray.lonInLong |= (locusArray.locusString[11] << 16);
	locusArray.lonInLong |= (locusArray.locusString[10] << 8);
	locusArray.lonInLong |= locusArray.locusString[9];
	exponent = (float)((locusArray.lonInLong >> 23) & 0xff);
	exponent -= 127.0;
	exponent = pow(2,exponent);
	mantissa = (float)(locusArray.lonInLong & 0x7fffff);
	mantissa = 1.0 + (mantissa/8388607.0);
	locusArray.longitude = mantissa * exponent;
	if ((locusArray.lonInLong & 0x80000000) == 0x80000000)
		locusArray.longitude = -locusArray.longitude;
	return;
}