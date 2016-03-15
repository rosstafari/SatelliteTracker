//---------------------------------------------------------------------------------
// TripodTracker
//
// This program is the main one of two (other:  Coprocessor.ino) that comprise
// the satellite tracking system comprised of a camera tripod, two stepper motors,
// and an Arrow UHF/VHF beam antenna.
//
// This program is responsible for the LCD (TFT) display and i2c communication
// with peripherals (RTC, digital compass).  Communication with the Coprocessor
// is performed over serial (SoftwareSerial library) at 9600 baud.
//
// Available TFT Colors:  
//
//  NAVY, DARKGREEN, DARKCYAN, MAROON, PURPLE, OLIVE, LIGHTGREY, DARKGREY,
//  BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, ORANGE, GREENYELLOW,
//  PINK
//
//  Don't forget to prefix the above with ILI9341_
// 
// TODO:  Set RTC to GMT so Julian Date is correct
//
//---------------------------------------------------------------------------------
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <DS1307.h>
#include "SoftwareSerial.h"

//--------------------------------------------------------------------
// CONSTANTS
//--------------------------------------------------------------------
#define VERSION "0.04"
#define TFT_DC 9
#define TFT_CS 10
#define RTC_HOURS_INDEX 2
#define RTC_MINUTES_INDEX 1 
#define RTC_SECONDS_INDEX 0
#define RTC_DAY_INDEX 4
#define RTC_MONTH_INDEX 5
#define RTC_YEAR_INDEX 6
#define ARD_TX_PIN 6 
#define ARD_RX_PIN 7 
#define BIG_BUFFER_SIZE 20
#define SMALL_BUFFER_SIZE 10
#define DAYS_IN_SIDEREAL_YEAR 365.256363004
#define DISPLAY_HEADER_HEIGHT 27

//--------------------------------------------------------------------
// GLOBALS
//--------------------------------------------------------------------
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/2nd Mini Pro
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];
int rtc[7]; 

//--------------------------------------------------------------------
// setup
//--------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  ss.begin(9600);
  initializeDisplay();
  delay(1000);
  startupTest();
  delay(3000);
  clearDisplayBody();
  float gmst = getGmSiderealTime();
  tft.setTextColor(ILI9341_GREENYELLOW); 
  tft.print("GMST: ");
  tft.println(gmst);

  while (gmst >= 360.0)
    gmst -= 360.0;
  
  tft.print("mod 360:");
  
  tft.println(gmst);
}

void gmst2()
{
  double M,Y,D,MN,H,S;
  double A,B,C,E,F;
  //fill integers with data from real time clock, or other wource
  M = rtc[RTC_MONTH_INDEX];
  Y = rtc[RTC_YEAR_INDEX]+2000;
  D = rtc[RTC_DAY_INDEX];
  MN = rtc[RTC_MINUTES_INDEX];
  H = rtc[RTC_HOURS_INDEX];
  S = (int) rtc[RTC_SECONDS_INDEX];
  if (M<3) {M+=12; Y-=1;}
  A = (long)Y/100;
  B = (long)A/4;
  C = (long)2-A+B;
  E = (long)(365.25*(Y+4716));
  F = (long)(30.6001*(M+1));
  double CurrentJDN = C+D+E+F-1524.5;
  //calculate terms required for LST calcuation and calculate GMST using an approximation
  double Current_d = CurrentJDN - 2451545.0;
  double Current_T = Current_d / 36525;
  double Term2=0.06570982441908*Current_d;
  
  double HourLastUpdatedJDN = H;
  double DayLastUpdatedJDN = D;
  
  H = H + ((double)MN/60) + ((double)S/3600);
  
  float GMST;
  float Term3;
  
  Term3=0.00273790935*H;
  Term3+=H;
  GMST = Term1 + Term2 + Term3;
  tft.println(GMST);
  //add on longitude to get LST
  // LST = GMST + location->GetLong_over15;
  LST = GMST + 0;
  
  //reduce it to 24 format
  int LSTint;
  LSTint = (int)LST;
  LSTint/=24;
  LST = LST - (double) LSTint * 24;  
}

//--------------------------------------------------------------------
// loop
//--------------------------------------------------------------------
void loop() 
{
  delay(5000);
}

//--------------------------------------------------------------------
// runClock
//--------------------------------------------------------------------
void runClock()
{
  while (1)
  {
    tft.setCursor(0, 30);
    displayDateTime();
    delay(1000);
    clearDisplayBody();
  }    
}

//--------------------------------------------------------------------
// startupTest
//--------------------------------------------------------------------
void startupTest()
{
  Serial.println("starting test");
  
  tft.print("RTC: ");
  displayDateTime();
  tft.setTextColor(ILI9341_YELLOW);  
  tft.println("Getting coP status...");
  int coPstatus = getCoprocessorStatus();
  tft.setTextColor(ILI9341_GREEN);  
  tft.println("Coprocessor online.");
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Testing AZ rotator...");
  turnAzRotator(45);
  delay(100);
  turnAzRotator(-45);
  tft.setTextColor(ILI9341_GREEN);  
  tft.println("Az rotation test complete.");
  tft.setTextColor(ILI9341_WHITE);  
}

//--------------------------------------------------------------------
// turnAzRotator
//--------------------------------------------------------------------
void turnAzRotator(int degrees)
{
  // TODO:  FIX THIS HARDCODING
  sendCoprocessorCommand();  
}

//--------------------------------------------------------------------
// sendCoprocessorCommand
//--------------------------------------------------------------------
void sendCoprocessorCommand()
{
  // TODO:  FIX THIS HARDCODING
  ss.println("RAF045:");
}

//--------------------------------------------------------------------
// getCoprocessorStatus
//--------------------------------------------------------------------
int getCoprocessorStatus()
{
  Serial.println("Sending RPT:");
  ss.println("RPT:");

  // WAIT FOR STAnnn: back
  do 
  {
    Serial.println("reading line");
    readNextArdCommLine();
    Serial.println("back");
  }    
  while (strncmp(bigBuffer, "STA", 3) != 0);

  strncpy(smallBuffer, bigBuffer + 3, 3);
  smallBuffer[3] = 0;
  tft.print("Status: ");
  tft.println(smallBuffer);
  // TODO:  Return actual status
  return 0;
}

//--------------------------------------------------------------------
// readNextArdCommLine
//--------------------------------------------------------------------
void readNextArdCommLine()
{
    int charsCopied = 0;
    
    do 
    {
      bigBuffer[charsCopied] = readArdCommChar();
      charsCopied++;
    }
    while (bigBuffer[charsCopied - 1] != ':');
    
    charsCopied--;
    bigBuffer[charsCopied] = 0;
}
  
//--------------------------------------------------------------------
// ReadArdCommChar
//--------------------------------------------------------------------
char readArdCommChar()
{
    Serial.println("checking availability");
    while (!ss.available())
    {
      delay(10);
    }
    Serial.println("it is!");

    Serial.print("+");
    char c;
    boolean goodChar = false;
    do
    {
      c = ss.read();  

      Serial.print(c);
      Serial.print('.');

      if (c == ':')
        goodChar = true;
      else if (c >= 'A' && c <= 'Z')
        goodChar = true;
      else if (c >= '0' && c <= '9')
        goodChar = true;
    }
    while (!goodChar);
    
    return c;
}

//--------------------------------------------------------------------
// displayDateTime
//--------------------------------------------------------------------
void displayDateTime()
{
  RTC.get(rtc, true);
  if (rtc[RTC_HOURS_INDEX] < 10)
    tft.print("0");
  tft.print(rtc[RTC_HOURS_INDEX]); 
  tft.print(":");
  if (rtc[RTC_MINUTES_INDEX] < 10)
    tft.print("0");
  tft.print(rtc[RTC_MINUTES_INDEX]);
  tft.print(":");
  if (rtc[RTC_SECONDS_INDEX] < 10)
    tft.print("0");
  tft.print(rtc[RTC_SECONDS_INDEX]);
  tft.print(" ");
  tft.print(rtc[RTC_MONTH_INDEX]);
  tft.print("/");
  tft.print(rtc[RTC_DAY_INDEX]);
  tft.print("/");
  tft.println(rtc[RTC_YEAR_INDEX]);
}

//--------------------------------------------------------------------
// initializeDisplay
//--------------------------------------------------------------------
void initializeDisplay()
{
  // Set up display 
  tft.begin();  
  tft.setRotation(1);
  clearDisplay();
}

//--------------------------------------------------------------------
// getGmSiderealTime
//--------------------------------------------------------------------
float getGmSiderealTime()
{
  RTC.get(rtc, true);
  int a = (14 - rtc[RTC_MONTH_INDEX]) / 12;
  int y = rtc[RTC_YEAR_INDEX] + 4800 - a;
  int m = rtc[RTC_MONTH_INDEX] + (12 * a) - 3;
  long jdn = rtc[RTC_DAY_INDEX] + (153*m + 2)/5 + (365L*(long)y) + y/4 - y/100 + y/400 - 32045;
  float jd = (float)jdn + ((float)rtc[RTC_HOURS_INDEX]-12.0)/24.0 + 
              (float)rtc[RTC_MINUTES_INDEX]/1440.0 + 
              (float)rtc[RTC_SECONDS_INDEX]/86400.0;
  // temp
  tft.println(jd);
  Serial.println(jd);
  
  float GMST = 18.697374558f + (24.06570982441908f * (jd - 2451545.0));
  return GMST;
}

//--------------------------------------------------------------------
// clearDisplay
//--------------------------------------------------------------------
void clearDisplay()
{
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

//--------------------------------------------------------------------
// clearDisplayBody
//--------------------------------------------------------------------
void clearDisplayBody()
{
  tft.fillRect(0, DISPLAY_HEADER_HEIGHT, tft.width(), tft.height() - DISPLAY_HEADER_HEIGHT, ILI9341_BLACK);
  tft.setCursor(0, DISPLAY_HEADER_HEIGHT + 5);
}

