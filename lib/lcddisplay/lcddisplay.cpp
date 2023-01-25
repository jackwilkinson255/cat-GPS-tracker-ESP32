#include <U8g2lib.h>
#include "lcddisplay.h"

U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED

void LCDDisplay::printMessage(char *msg){
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0,20,msg);
  } while ( u8g2.nextPage() );
  delay(1000);

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0,30,"From Black Beard");
  } while ( u8g2.nextPage() );
  delay(1000);
}

void LCDDisplay::setupLCD(void){
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);
}