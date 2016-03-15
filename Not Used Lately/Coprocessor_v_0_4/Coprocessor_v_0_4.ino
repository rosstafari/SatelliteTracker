#include <Stepper.h>
#include "SoftwareSerial.h"

//--------------------------------------------------------------------
// DEFINES
//--------------------------------------------------------------------
#define STEPS 200 // change this to the number of steps on your motor
#define SPEED 10
#define ARD_TX_PIN 2 
#define ARD_RX_PIN 3 
#define BIG_BUFFER_SIZE 100
#define SMALL_BUFFER_SIZE 10
#define GPS_RX_PIN 4

//--------------------------------------------------------------------
// GLOBALS
//--------------------------------------------------------------------long loopCount;
Stepper stepper(STEPS, 5, 6, 7, 8); // The four #s are Arduino pins for IN1, IN2, IN3, IN4
SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/main Mini Pro
SoftwareSerial gss(GPS_RX_PIN, 0, true);
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];
boolean gpsOkay;
boolean gpsHasSats;
int numGpsLinesRead;

enum Command
{
  CMD_RESET,
  CMD_REPORT_STATUS,
  CMD_ROTATE_AZ_FWD,
  CMD_ROTATE_AZ_BKWD,
  CMD_SEND_DECIMAL_LATITUDE,
  CMD_UNKNOWN
};

//--------------------------------------------------------------------
// setup
//--------------------------------------------------------------------
void setup() 
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);

  gpsOkay = false;
  gpsHasSats = false;
  numGpsLinesRead = 0;
  ss.begin(9600);
}

//--------------------------------------------------------------------
// loop
//--------------------------------------------------------------------
void loop() 
{
  int cmd = waitForCommand();

  switch (cmd)
  {
    case CMD_REPORT_STATUS:
      ss.end();
      gss.begin(4800);
      testGps();
      gss.end();
      ss.begin(9600);
      
      if (!gpsOkay)
      {
        Serial.println("Sending back STA009:");
        ss.println("STA009:");        
      }
      else if (!gpsHasSats)
      {
        Serial.println("Sending back STA001:");
        ss.println("STA001:");        
      }
      else
      {
        Serial.println("Sending back STA000:");
        ss.println("STA000:");
      }
      
      break;
      
    case CMD_ROTATE_AZ_FWD:
      int deg = atoi(smallBuffer);
      rotateAzDelta(SPEED, deg);
      break;
  }
  delay(500);    
}

//--------------------------------------------------------------------
// testGps
//--------------------------------------------------------------------
void testGps()
{
  Serial.println("Testing GPS receiver... "); 
  
  for (int i=0; i<30; i++)
  {
    GetNextGpsLine();
    Serial.println(bigBuffer);

    if (strlen(bigBuffer) > 20 && bigBuffer[0] == '$')
    {
      Serial.println("Got a good GPS receiver");
      gpsOkay = true;
      Serial.println("Checking on satellites...");

      do
      {
        GetNextGpsLine();
        Serial.println(bigBuffer);
      }
      while (strncmp(bigBuffer+1, "GPGGA", 5) != 0);

      Serial.println("Got a GPGGA");
      Serial.println("Reading field...");
      GetNthField(bigBuffer, smallBuffer, 8);
      Serial.print("Field: -->");
      Serial.print(smallBuffer);
      Serial.println("<--");

      if (strcmp(smallBuffer, "00") == 0)
      {
        Serial.println("No satellites available");
      }
      else
      {
        Serial.print("GOT ");
        Serial.print(smallBuffer);
        Serial.println(" SATELLITES!");
        gpsHasSats = true;
      }
      break;
    }
  }
}

//--------------------------------------------------------------------
// GetNextGpsLine
//--------------------------------------------------------------------
void GetNextGpsLine()
{
    char c;
  
    do 
    {
      c = ReadGpsChar();
    }
    while (c != '$');
  
    bigBuffer[0] = '$';
    int charsCopied = 1;
  
    do
    {
      c = ReadGpsChar();
          
      if (c != '\n' && c != '\r' && c!= '$' && charsCopied < BIG_BUFFER_SIZE - 1)
      {
        bigBuffer[charsCopied] = c;
        charsCopied++;
      }
    }
    while (c != '\n' && c != '\r' && c != '$');
  
    bigBuffer[charsCopied] = 0;
    numGpsLinesRead++;
}

//--------------------------------------------------------------------
// ReadGpsChar
//--------------------------------------------------------------------
char ReadGpsChar()
{
    while (!gss.available())
      delay(10);
    
    return gss.read();  
}

//--------------------------------------------------------------------
// GetNthField
//--------------------------------------------------------------------
void GetNthField(char * src, char * dest, int fieldNum)
{
  int currentFieldNum = 1;
  int currentPosn = 0;
  
  while (currentFieldNum < fieldNum)
  {
    do
    {
      currentPosn++;
    } 
    while (currentPosn < strlen(src) && src[currentPosn] != ',');
    currentFieldNum++; 
  }

  if (currentPosn >= strlen(src))
  {
    Serial.print("      Terminating at -->0");
    *dest = '\0';
    Serial.print("   ");
    Serial.println(currentPosn);
    return;
  }
  
  if (src[currentPosn] == ',')
    currentPosn++;
    
  int destPosn = 0;
  while (currentPosn < strlen(src) && src[currentPosn] != ',')
  {
    Serial.print("      Copying over -->");
    Serial.println(src[currentPosn]);
     dest[destPosn] = src[currentPosn];
     destPosn++;
     currentPosn++;
  }
  
    Serial.print("      Terminating at -->");
    Serial.println(destPosn);
  dest[destPosn] = '\0';
    Serial.print("   ");
    Serial.println(currentPosn);
  
}
//--------------------------------------------------------------------
// rotateAzDelta
//--------------------------------------------------------------------
void rotateAzDelta(int speed, int degrees)
{
  Serial.print("Rotating forward "); 
  Serial.print(degrees);
  Serial.println(" degrees");
  
  stepper.setSpeed(speed);
  stepper.step((-1.0*degrees)/1.8); 
}

//--------------------------------------------------------------------
// waitForCommand
//--------------------------------------------------------------------
int waitForCommand()
{
  Serial.println("Waiting on command from main ard...");
  readNextArdCommLine();

  if (strcmp(bigBuffer, "RPT") == 0)
  {
    Serial.println("GOT RPT!!");
    return CMD_REPORT_STATUS;
  }
  
  else if (strncmp(bigBuffer, "RAF", 3) == 0)
  {
    Serial.print("Got ");
    Serial.println(bigBuffer);

    strncpy(smallBuffer, bigBuffer + 3, 3);
    smallBuffer[3] = 0;
    Serial.print("Deg.:");
    Serial.println(smallBuffer);
    return CMD_ROTATE_AZ_FWD;
  }

  else
  {
    Serial.print("GOT-->");
    Serial.print(bigBuffer);
    Serial.println("<--");
    Serial.print("Length: ");
    Serial.println(strlen(bigBuffer));
    Serial.print("bb[0]: ");
    Serial.println((int)bigBuffer[0]);
    return CMD_UNKNOWN;
  }
  
  //return CMD_UNKNOWN;
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
      else if (c >= '0' && c <= '9')
        goodChar = true;
    }
    while (!goodChar);
    
    return c;
}

