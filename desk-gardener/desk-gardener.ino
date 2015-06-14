#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

const char* monthname[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char* dayname[] = {"Mon","Tue","Wed","Thu","Fri"};
const char* colorname[] = {"Off","Red","Green","Yellow","Blue","Violet","Teal","White"};
uint8_t menu = 0;
const uint8_t menucount = 2;
uint8_t lcdcolor = 0x7;
uint8_t buttons;

RTC_DS1307 rtc;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  Wire.begin();
  rtc.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(lcdcolor);
}

void loop() {

  buttons = lcd.readButtons();

  if (buttons & BUTTON_SELECT) {
    menu = (menu + 1) % menucount;
    lcd.clear();
  }

  if (menu == 1) {
    lcd.setCursor(0,0);
    lcd.print("Backlight color");
    lcd.setCursor(0,1);
    lcd.print("<");
    lcd.print(lcdcolor);
    lcd.print("> ");
    lcd.print(colorname[lcdcolor]);
    
    if (buttons & BUTTON_RIGHT) {
      lcdcolor = (lcdcolor + 1) % 8;
      lcd.setBacklight(lcdcolor);
      lcd.clear();
    }
    
    if (buttons & BUTTON_LEFT) {
      lcdcolor = lcdcolor ? lcdcolor - 1 : 7;
      lcd.setBacklight(lcdcolor);
      lcd.clear();
    }
  }

  if (menu == 0) {
    printdatetime();
  }

}

void printdatetime() {

  DateTime now = rtc.now();

  lcd.setCursor(0,0);
  lcd.print(dayname[now.dayOfWeek()]);
  lcd.print(" ");
  printzeroed(now.day());
  lcd.print(" ");
  lcd.print(monthname[now.month()-1]);
  lcd.print(" ");
  printzeroed(now.year());

  lcd.setCursor(0,1);
  printzeroed(now.hour());
  lcd.print(":");
  printzeroed(now.minute());
  lcd.print(":");
  printzeroed(now.second());

}

void printzeroed(int x) {

  if(x<10) lcd.print("0");
  lcd.print(x);

}

