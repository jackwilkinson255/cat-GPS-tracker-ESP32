#include <Arduino.h>
#include <unity.h>
#include <web_utils.h>
#include <WiFi.h>
#include <WebSerial.h>



WebUtils web;

// String recvMsg(uint8_t *data, size_t len){
// //   WebSerial.println("Received Data...");
//   String d = "";
//   for(int i=0; i < len; i++){
//     CALLBACK_MSG += char(data[i]);

//   }
//   WebSerial.println(d);
//   return d;
// }

void setUp(void) {
    // set stuff up here
    Serial.begin(115200);
}

void tearDown(void) {
    // clean stuff up here
}

void test_wifi_available(void){
    char target_wifi[] = "BTHub6-GPSR";
    bool network_found = web.findTargetNetwork(target_wifi);
    TEST_ASSERT_TRUE(network_found);
}

void test_wifi_connection(void){
    TEST_ASSERT_TRUE(web.connectWifi());
}

void test_web_serial(void){
    char test_str[] = "Hello World! 1234";
    web.connectWebSerialServer();
    web.printlnWebSerial(test_str);
    // WebSerial.println(test_str);

    // TODO here loop through
    uint8_t * pCallback = web.pCallback;
    size_t lenCallback = web.lenCallback;

    char callback_str[lenCallback+1];

    for (size_t i = 0; i < lenCallback; i++)
    {
        callback_str[i] = pCallback[i];
        Serial.println("Callback:");
        Serial.println(callback_str[i]);
    }

    callback_str[lenCallback+1] = '\0';

    TEST_ASSERT_EQUAL_STRING(test_str, callback_str);

}

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_available);
    RUN_TEST(test_wifi_connection);
    // TODO: having issues with serial test
    RUN_TEST(test_web_serial);
    UNITY_END();
}

void setup() {
    delay(2000);

    RUN_UNITY_TESTS();
}


// void setup()
// {
    // delay(2000); // service delay
    // UNITY_BEGIN();
    // RUN_TEST(test_wifi_available);

    // RUN_TEST(test_string_concat);

    // RUN_TEST(test_string_replace);

    // UNITY_END(); // stop unit testing
    // RUN_TEST(test_string_substring);
    // RUN_TEST(test_string_index_of);
    // RUN_TEST(test_string_equal_ignore_case);
    // RUN_TEST(test_string_to_upper_case);
//}





void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    while(1){
        delay(500);
    }

}



// TODO: test battery

// TODO: test wifi credentials

// TODO: test wifi connection

// TODO: test

// TODO: test

// TODO: test

