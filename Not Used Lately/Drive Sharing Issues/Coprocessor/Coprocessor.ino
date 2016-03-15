#include <Stepper.h>
#include "SoftwareSerial.h"

#define STEPS 200 // change this to the number of steps on your motor
#define SPEED 10
#define ARD_TX_PIN 2 
#define ARD_RX_PIN 3 
#define BIG_BUFFER_SIZE 100
#define SMALL_BUFFER_SIZE 10

long loopCount;
Stepper stepper(STEPS, 5, 6, 7, 8); // The four #s are Arduino pins for IN1, IN2, IN3, IN4
SoftwareSerial ss(ARD_RX_PIN, ARD_TX_PIN); // For comm w/main Mini Pro
char bigBuffer[BIG_BUFFER_SIZE];
char smallBuffer[SMALL_BUFFER_SIZE];

enum Command
{
  CMD_RESET,
  CMD_REPORT_STATUS,
  CMD_ROTATE_AZ_FWD,
  CMD_ROTATE_AZ_BKWD,
  CMD_UNKNOWN
};


void setup() 
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  ss.begin(9600);
}

void loop() 
{
  int cmd = waitForCommand();

  switch (cmd)
  {
    case CMD_REPORT_STATUS:
      Serial.println("Sending back STA000:");
      ss.println("STA000:");
      break;
      
    case CMD_ROTATE_AZ_FWD:
      int deg = atoi(smallBuffer);
      rotateAzDelta(SPEED, deg);
      break;
  }
  delay(500);    
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

    Serial.print("+");
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

