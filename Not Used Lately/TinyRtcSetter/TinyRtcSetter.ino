#include <Wire.h>
#include <DS1307.h>

#define RTC_HOURS_INDEX 2
#define RTC_MINUTES_INDEX 1 
#define RTC_SECONDS_INDEX 0
#define RTC_DAY_INDEX 4
#define RTC_MONTH_INDEX 5
#define RTC_YEAR_INDEX 6

String comdata = "";
int mark=0;
int rtc[7];
byte rr[7];

void setup()
{
  DDRC |= _BV(2) | _BV(3); // POWER:Vcc Gnd
  PORTC |= _BV(3); // VCC PINC3
  Serial.begin(9600);
  Serial.println("Ross is testing the RTC...");
  Serial.println("Reading the clock...");
  displayDateTime();
  Serial.println("Setting the clock...");
  setDateTime(3, 5, 2016, 21, 49, 55);
  Serial.println("Reading the clock...");
  displayDateTime();
  
}

//--------------------------------------------------------------------
// setDateTime
//--------------------------------------------------------------------
void setDateTime(int m, int d, int y, int h, int mm, int s)
{  
  RTC.stop();
  RTC.set(DS1307_SEC, s);
  RTC.set(DS1307_MIN, mm);
  RTC.set(DS1307_HR, h);
  RTC.set(DS1307_DOW, 6);
  RTC.set(DS1307_DATE, d);
  RTC.set(DS1307_MTH, m);
  RTC.set(DS1307_YR, y);
  RTC.start();
}

//--------------------------------------------------------------------
// displayDateTime
//--------------------------------------------------------------------
void displayDateTime()
{
  RTC.get(rtc, true);
  if (rtc[RTC_HOURS_INDEX] < 10)
    Serial.print("0");
  Serial.print(rtc[RTC_HOURS_INDEX]); 
  Serial.print(":");
  if (rtc[RTC_MINUTES_INDEX] < 10)
    Serial.print("0");
  Serial.print(rtc[RTC_MINUTES_INDEX]);
  Serial.print(":");
  if (rtc[RTC_SECONDS_INDEX] < 10)
    Serial.print("0");
  Serial.print(rtc[RTC_SECONDS_INDEX]);
  Serial.print(" ");
  Serial.print(rtc[RTC_MONTH_INDEX]);
  Serial.print("/");
  Serial.print(rtc[RTC_DAY_INDEX]);
  Serial.print("/");
  Serial.println(rtc[RTC_YEAR_INDEX]);
}

void loop()
{
  delay(500);
}
