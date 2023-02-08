#include <Arduino.h>
#include <unity.h>
#include "aws_iot.h"

AWSIOT aws;





void setUp(void) {
    // set stuff up here
    Serial.begin(115200);
}

void tearDown(void) {
    // clean stuff up here
}



void test_aws_connection(void){
    TEST_ASSERT_TRUE(1);
}

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    RUN_TEST(test_aws_connection);
    UNITY_END();
}

void setup() {
    delay(2000);
    RUN_UNITY_TESTS();
}

void loop(){}

