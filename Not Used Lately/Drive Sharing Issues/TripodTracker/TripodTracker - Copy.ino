#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <DS1307.h>
#include <Stepper.h>

#define STEPS 200 // change this to the number of steps on your motor
#define VERSION "0.01"
#define TFT_DC 9
#define TFT_CS 10

// I think these are reversed
#define RTC_MONTH_INDEX 4
#define RTC_DAY_INDEX 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Stepper stepper(STEPS, 3, 6, 7, 8); // The four #s are Arduino pins for IN1, IN2, IN3, IN4

int rtc[7]; 

void setup() 
{
  initializeDisplay();
  delay(2000);
  tft.print("RTC: ");
  displayDateTime();
  float jd = getJulianDay();
  tft.print("Testing tracker...");
  rotateAzDelta(10,90);
  delay(1000);
  rotateAzDelta(10,-90);
  tft.println("done");
}

void loop() 
{
  delay(5000);
}

void rotateAzDelta(int speed, int degrees)
{
  stepper.setSpeed(speed);
  stepper.step((-1.0*degrees)/1.8); 
}

void displayDateTime()
{
  RTC.get(rtc, true);
  tft.print(rtc[2]); // H
  tft.print(":");
  tft.print(rtc[1]); // M
  tft.print(":");
  if (rtc[0] < 10)
    tft.print("0");
  tft.print(rtc[0]); // S
  tft.print(" ");
  tft.print(rtc[RTC_MONTH_INDEX]); // M
  tft.print("/");
  tft.print(rtc[RTC_DAY_INDEX]); // D
  tft.print("/");
  tft.println(rtc[6]);  // Y
}

void initializeDisplay()
{
  // Set up display 
  tft.begin();  
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_ORANGE);  
  tft.setTextSize(2);
  tft.print("WAC TripodTracker v");
  tft.println(VERSION);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);  
  tft.println("_____________________________________________________");
  tft.println();
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(2);
}

float getJulianDay()
{
  RTC.get(rtc, true);
  int a = (14 - rtc[RTC_MONTH_INDEX]) / 12;
  int y = rtc[6] + 4800 - a;
  int m = rtc[RTC_MONTH_INDEX] + (12 * a) - 3;
  long jdn = rtc[RTC_DAY_INDEX] + (153*m + 2)/5 + (365L*(long)y) + y/4 - 32083;
  float jd = (float)jdn + ((float)rtc[2]-12.0)/24.0 + (float)rtc[1]/1440.0 + (float)rtc[0]/86400.0;
  /*
  tft.print("a = ");
  tft.println(a);
  tft.print("y = ");
  tft.println(y);
  tft.print("m = ");
  tft.println(m);
  */
  tft.print("jdn = ");
  tft.println(jdn);
  tft.print("jd = ");
  tft.println(jd);

  return 0.0f;
}

