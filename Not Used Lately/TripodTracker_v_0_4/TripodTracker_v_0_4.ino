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
boolean needToWaitForGps;

enum CoPstatus
{
  CoP_STATUS_OK,
  CoP_STATUS_GPS_NOT_FOUND,
  CoP_STATUS_GPS_WITHOUT_SATELLITES
};

//--------------------------------------------------------------------
// setup
//--------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  ss.begin(9600);
  initializeDisplay();
  delay(1000);
  needToWaitForGps = false;
  startupTest();

  /*
  if (needToWaitForGps)
    waitForGps();
  */  
    
  
  /*
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
  */
}

//--------------------------------------------------------------------
// waitForGps
//--------------------------------------------------------------------
void waitForGps()
{
    tft.println("Going to GPS page in 5...");
    int coPstatus;
    
    do
    {
      delay(5000);
      clearDisplayBody();
      coPstatus = getCoprocessorStatus();

      if (coPstatus != CoP_STATUS_OK)
        tft.print("GPS not ready");
      else
        tft.print("GPS LOCK!!");
    }
    while (coPstatus != CoP_STATUS_OK);

    getGpsPosn();
}

//--------------------------------------------------------------------
// getGpsPosn
//--------------------------------------------------------------------
void getGpsPosn()
{
  Serial.println("Sending LAT:");
  ss.println("LAT:");

  // WAIT FOR <decimal latitude>: back
  Serial.println("reading line");
  readNextArdCommLine();
  Serial.println("back");
  Serial.print("got: ");
  Serial.println(bigBuffer);    
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
  tft.setTextColor(ILI9341_GREENYELLOW);  
  tft.println("Getting coP status...");
  int coPstatus = getCoprocessorStatus();
  tft.setTextColor(ILI9341_LIGHTGREY);  
  tft.print("Status: ");
 
  switch (coPstatus)
  {
    case CoP_STATUS_OK:
      tft.setTextColor(ILI9341_GREEN);  
      tft.println("AOK");
      break;
    case CoP_STATUS_GPS_NOT_FOUND:
      tft.setTextColor(ILI9341_RED);  
      tft.println("GPS not found");
      needToWaitForGps = true;
      break;
    case CoP_STATUS_GPS_WITHOUT_SATELLITES:
      tft.setTextColor(ILI9341_YELLOW);  
      tft.println("GPS w/o sats");
      needToWaitForGps = true;
      break;
    default:
      tft.println("UNKNOWN");  
  }
  
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Testing AZ rotator...");
  turnAzRotator(45);
  delay(100);
  turnAzRotator(-45);
  tft.setTextColor(ILI9341_GREEN);  
  tft.println("Az rotation test complete.");
  tft.setTextColor(ILI9341_LIGHTGREY);  
  float gmst = getDecimalGmSiderealTime();
  tft.print("Decimal GMST: ");
  tft.println(gmst);

    int GMSTh = (int)gmst;
    float GMSTright = gmst - (float)GMSTh;
    float GMSTm = 60.0*GMSTright;
    float mright = GMSTm - (int)GMSTm;
    float GMSTs = mright * 60.0;
  
  tft.print("GMST: ");
  tft.print((int)gmst);
  tft.print(":");
  tft.print((int)GMSTm);
  tft.print(":");
  tft.println(GMSTs);
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
    Serial.print("got: ");
    Serial.println(bigBuffer);
  }    
  while (strncmp(bigBuffer, "STA", 3) != 0);

  strncpy(smallBuffer, bigBuffer + 3, 3);
  smallBuffer[3] = 0;

  Serial.print("code: ");
  Serial.println(smallBuffer);
  if (strcmp(smallBuffer, "009") == 0)
    return CoP_STATUS_GPS_NOT_FOUND;
  if (strcmp(smallBuffer, "001") == 0)
    return CoP_STATUS_GPS_WITHOUT_SATELLITES;
  return CoP_STATUS_OK;
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
double getDecimalGmSiderealTime()
{
  RTC.get(rtc, true);
  int a = (14 - rtc[RTC_MONTH_INDEX]) / 12;
  int y = rtc[RTC_YEAR_INDEX] + 4800 - a;
  int m = rtc[RTC_MONTH_INDEX] + (12 * a) - 3;

  long jdn = rtc[RTC_DAY_INDEX] + (153*m + 2)/5 + (365L*(long)y) + y/4 - y/100 + y/400 - 32045;
  double jd = (double)jdn + 
              (((double)rtc[RTC_HOURS_INDEX])-12.0)/24.0 + 
              (double)rtc[RTC_MINUTES_INDEX]/1440.0 + 
              (double)rtc[RTC_SECONDS_INDEX]/86400.0;

  
  double D = jd - 2451545.0;
  tft.print("D: ");
  tft.println(D);
//  double GMST = 18.697374558 + 24.06570982441908 * D;
  double GMST = 18.7 + D*24.066;
  tft.print("D*#: "); 
  tft.println( 24.066 * D);
  tft.print("Raw gmst: ");
  tft.println(GMST);
  double rawgmst = GMST;
  Serial.print("Raw gmst: ");
  Serial.println(GMST);
  /*  doesn't work right
  while (GMST >= 24.0)
    GMST -= 24.0;   
  */

    double gleft = (long)GMST * 1.0;
    double gright = GMST - gleft;
    long mod = ((long)gleft) % 24;
    tft.print((long)gleft);
    tft.print(" % 24 = ");
    tft.println(mod);
    
    GMST = mod * 1.0 + gright;
   
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


//--------------------------------------------------------------------
// loop
//--------------------------------------------------------------------
void loop() 
{
  delay(5000);
}
