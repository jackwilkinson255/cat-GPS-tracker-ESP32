
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "misc_utils.h"


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 86400         /* Time ESP32 will go to sleep (in seconds) */




void DeepSleep::enterDeepSleep(void)
{
    // Serial.println("Going to sleep forever");
    // Serial.flush();
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // esp_deep_sleep_start();
    // Serial.println("This will never be printed");

}


void DeepSleep::enterDeepSleep(int num_secs)
{
    esp_sleep_enable_timer_wakeup(num_secs * uS_TO_S_FACTOR);
    Serial.print("Going to sleep for ");
    Serial.print(num_secs);
    Serial.println("s");
    Serial.flush();
    esp_deep_sleep_start();
    Serial.println("This will never be printed");

}


// void NeoPix::off(void)
// {
//     p_pixels->clear();
//     p_pixels->show();
// }



// def get_voltage(pin):
//     return (pin.value * 3.3) / 65536