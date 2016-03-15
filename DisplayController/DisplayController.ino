//---------------------------------------------------------------------------------
// DisplayController
//
// This program is the TFT display controller that works with a main processor that comprise
// the satellite tracking system comprised of a camera tripod, two stepper motors,
// and an Arrow UHF/VHF beam antenna.
//
// Communication with the Coprocessor
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
#include "SoftwareSerial.h"

//--------------------------------------------------------------------
// CONSTANTS
//--------------------------------------------------------------------
#define VERSION "0.05"
#define TFT_DC 9
#define TFT_CS 10
#define ARD_TX_PIN 6 
#define ARD_RX_PIN 7 
#define BIG_BUFFER_SIZE 100
#define SMALL_BUFFER_SIZE 50
#define DISPLAY_HEADER_HEIGHT 27

//--------------------------------------------------------------------
// GLOBALS
//--------------------------------------------------------------------
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/2nd Mini Pro
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];

//--------------------------------------------------------------------
// setup
//--------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  ss.begin(9600);
  Serial.println("running...");
  initializeDisplay();
}

//--------------------------------------------------------------------
// loop
//--------------------------------------------------------------------
void loop() 
{
  readNextArdCommLine();
  Serial.println(bigBuffer);
  getCommand();
  Serial.println(smallBuffer);

  if (strcmp(smallBuffer, "PLN") == 0)
  {
    Serial.println("That's a println command!!");
    char * tok = toksplit(bigBuffer, '^', smallBuffer, SMALL_BUFFER_SIZE - 1);
    tok = toksplit(tok, '^', smallBuffer, SMALL_BUFFER_SIZE - 1);
    tok = toksplit(tok, '^', smallBuffer, SMALL_BUFFER_SIZE - 1);
    tok = toksplit(tok, '^', smallBuffer, SMALL_BUFFER_SIZE - 1);
    printLine();
  }
  else
    Serial.println("UNKNOWN COMMAND");
}

//----------------------------------------------------------------------------------------------
// printLine
// ----------------------------------------------------------------------------------------------
void printLine()
{
  // Display what's in smallBuffer and go to next line
  tft.println(smallBuffer);
}

//----------------------------------------------------------------------------------------------
// getCommand
// ----------------------------------------------------------------------------------------------
void getCommand()
{
  // Gets first field from bigBuffer and puts in smallBuffer
  toksplit(bigBuffer, '^', smallBuffer, SMALL_BUFFER_SIZE - 1);
}

//----------------------------------------------------------------------------------------------
// toksplit
// ----------------------------------------------------------------------------------------------
char * toksplit(char *src,      /* Source of tokens */ 
                     char tokchar,    /* token delimiting char */ 
                     char *token,  /* receiver of parsed token */ 
                     size_t lgh)   /* length token can receive */ 
                                   /* not including final '\0' */ 
{ 
   if (src) { 
      while (' ' == *src) src++; 


      while (*src && (tokchar != *src)) { 
         if (lgh) { 
            *token++ = *src; 
            --lgh; 
         } 
         src++; 
      } 
      if (*src && (tokchar == *src)) src++; 
   } 
   *token = '\0'; 
   return src; 



} /* toksplit */ 



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
    while (bigBuffer[charsCopied - 1] != '~');
    
    charsCopied--;
    bigBuffer[charsCopied] = 0;
}
  
//--------------------------------------------------------------------
// ReadArdCommChar
//--------------------------------------------------------------------
char readArdCommChar()
{
    while (!ss.available())
    {
      delay(10);
    }

    char c;
    boolean goodChar = false;
    do
    {
      c = ss.read();  
      
      if (c == ':' || c == ',' || c == '^' || c == '~' || c == '.' || c == ' ')
        goodChar = true;
      else if (c >= 'A' && c <= 'Z')
        goodChar = true;
      else if (c >= 'a' && c <= 'z')
        goodChar = true;
      else if (c >= '0' && c <= '9')
        goodChar = true;
    }
    while (!goodChar);
    
    return c;
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



