#include <Arduino.h>
#include <unity.h>
#include "gps_utils.h"
#include "mocking.h"
#include "web_utils.h"

HardwareSerial *pSerial2 = &Serial2;
GPS_Utils gps(pSerial2);
GPS_Utils_Mock gps_m;
WebUtils web;

void setUp(void) {
    Serial.begin(115200);
    gps.setup(erase); // Erase data to begin with
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    gps.setup(logging);

    // Collect data for 10s
    uint32_t start_time = millis(), current_time;
    while((current_time - start_time < 10000) || !gps.fix){
        Serial.println("Gathering data...");
        gps.loop();
        current_time = millis();
    }
}

void tearDown(void) {
    // clean stuff up here
}


void test_gps_config_set(void){
    // Check interval set to 1 Hz
    gps.LOCUS_ReadStatus();
    TEST_ASSERT_EQUAL_INT(1, gps.LOCUS_interval);
}

void test_gps_has_fix(void){
    // Only run the following tests when you have a fix
    TEST_ASSERT_TRUE(bool(gps.fix));
    // TEST_ASSERT_TRUE(1);
}


void test_timestamp_correct(void){
    web.setup();
    delay(1000);
    web.updateTimestamp();
    gps.loop();
    TEST_ASSERT_EQUAL_UINT(gps.year, web.year);
    TEST_ASSERT_EQUAL_UINT(gps.month, web.month);
    TEST_ASSERT_EQUAL_UINT(gps.day, web.day);
    TEST_ASSERT_EQUAL_UINT(gps.hour, web.hour);
    TEST_ASSERT_EQUAL_UINT(gps.minute, web.minute);
}




void test_locus_has_logged_data(void){
    // Access LOCUS data, could check the date
    gps.setup(dump);

    gps.loop();


}

void test_locus_erase(void){
    // Erase data
    gps.setup(erase);
    gps.loop();

    // Check LOCUS status for percentage of data used
    gps.LOCUS_ReadStatus();
    Serial.println(gps.LOCUS_config);

}


void test_timestamp_correct_format(void)
{
    // Case 1
    gps_m.hour = 17, gps_m.minute = 2, gps_m.seconds = 45, gps_m.year = 22, gps_m.month = 12, gps_m.day = 22;
    gps_m.getTimestamp();
    TEST_ASSERT_EQUAL_STRING("22-12-22 17:02:45", gps_m.timestamp);

    // Case 2
    gps_m.hour = 1, gps_m.minute = 2, gps_m.seconds = 3, gps_m.year = 22, gps_m.month = 4, gps_m.day = 5;
    gps_m.getTimestamp();
    TEST_ASSERT_EQUAL_STRING("22-04-05 01:02:03", gps_m.timestamp);
}


void test_location_correct_format(void)
{
    // Case 1
    gps_m.latitude = 1234.56, gps_m.longitude = 98765.43;
    gps_m.lat = 'N', gps_m.lon = 'E';
    gps_m.getLocation();
    TEST_ASSERT_EQUAL_STRING("+12 34.56', +987 65.43'", gps_m.location);

    // Case 2
    gps_m.latitude = 1972.10, gps_m.longitude = 4392.29;
    gps_m.lat = 'S', gps_m.lon = 'W';
    gps_m.getLocation();
    TEST_ASSERT_EQUAL_STRING("-19 72.10', -43 92.29'", gps_m.location);

    // Case 3
    gps_m.latitude = 4827.12, gps_m.longitude = 123.39;
    gps_m.lat = 'S', gps_m.lon = 'E';
    gps_m.getLocation();
    TEST_ASSERT_EQUAL_STRING("-48 27.12', +1 23.39'", gps_m.location);
}


void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    RUN_TEST(test_gps_config_set);
    RUN_TEST(test_gps_has_fix);
    RUN_TEST(test_timestamp_correct);
    RUN_TEST(test_timestamp_correct_format);
    RUN_TEST(test_location_correct_format);
    // RUN_TEST(test_locus_has_logged_data);
    UNITY_END();
}

void setup() {
    delay(2000);
    RUN_UNITY_TESTS();
}

void loop(){}