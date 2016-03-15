#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <DS1307.h>
#include "SoftwareSerial.h"

#define VERSION "0.02"
#define TFT_DC 9
#define TFT_CS 10
#define RTC_DAY_INDEX 4
#define RTC_MONTH_INDEX 5
#define ARD_TX_PIN 6 
#define ARD_RX_PIN 7 
#define BIG_BUFFER_SIZE 20
#define SMALL_BUFFER_SIZE 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/2nd Mini Pro
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];
int rtc[7]; 

void setup() 
{
  Serial.begin(9600);
  pinMode(ARD_RX_PIN, INPUT);
  pinMode(ARD_TX_PIN, OUTPUT);  
  ss.begin(9600);
  initializeDisplay();
  delay(1000);
  startupTest();
}

void loop() 
{
  delay(5000);
}

//--------------------------------------------------------------------
// startupTest
//--------------------------------------------------------------------
void startupTest()
{
  Serial.println("starting test");
  
  tft.print("RTC: ");
  displayDateTime();
  float jd = getJulianDay();
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
      else if (c >= '0' && c >= '9')
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

//--------------------------------------------------------------------
// initializeDisplay
//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
// ReadArdCommChar
//--------------------------------------------------------------------
float getJulianDay()
{
  RTC.get(rtc, true);
  int a = (14 - rtc[RTC_MONTH_INDEX]) / 12;
  int y = rtc[6] + 4800 - a;
  int m = rtc[RTC_MONTH_INDEX] + (12 * a) - 3;
  long jdn = rtc[RTC_DAY_INDEX] + (153*m + 2)/5 + (365L*(long)y) + y/4 - 32083;
  float jd = (float)jdn + ((float)rtc[2]-12.0)/24.0 + (float)rtc[1]/1440.0 + (float)rtc[0]/86400.0;
  float GMST = 18.697374558f + 24.06570982441908f * jd;
  
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
  tft.print("GMST = ");
  tft.println(GMST);
  
  return 0.0f;
}

