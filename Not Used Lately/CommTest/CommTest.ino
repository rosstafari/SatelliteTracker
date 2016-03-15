#include "SoftwareSerial.h"

#define ARD_TX_PIN 6 
#define ARD_RX_PIN 7 
#define BIG_BUFFER_SIZE 100
#define SMALL_BUFFER_SIZE 10

SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/main Mini Pro
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];
long count;

void setup() 
{
  Serial.begin(9600);
  ss.begin(9600);
  count=0;
}

void loop() 
{
  strcpy(bigBuffer, "ABC:");
  itoa(count, bigBuffer + 4, 10);
  Serial.print("Sending ");
  Serial.println(bigBuffer);
  ss.println(bigBuffer);
  count++;
  Serial.println("Reading...");
  readNextArdCommLine();
  Serial.print("Got ");
  Serial.println(bigBuffer);
  delay(500);    
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
  
    while (!ss.available())
    {
      delay(10);
    }

    char c;
    boolean goodChar = false;
    do
    {
      c = ss.read();  

      if (c == ':')
        goodChar = true;
      else if (c >= 'A' && c <= 'Z')
        goodChar = true;
      else if (c >= '0' && c >= '9')
        goodChar = true;
    }
    while (!goodChar);
    
    Serial.print(c);
    Serial.print(".");
    return c;
}

